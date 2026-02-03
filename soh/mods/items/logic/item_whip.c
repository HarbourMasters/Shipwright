/**
 * item_whip.c - Whip from Spirit Tracks
 *
 * Controls:
 *   C Button:         Lash whip forward (attack/grapple)
 *   Analog (swinging): Control pendulum swing direction
 *   Release C:         Launch from swing with momentum
 *
 * Features:
 *   - Grapples beam/bar shaped surfaces for pendulum swing
 *   - Combat: paralyze enemies, pull shields, disarm
 *   - Can grab certain actors and items
 *   - Momentum-based release for traversal
 */

#include "z64.h"
#include "item_whip.h"
#include "../custom_items.h"
#include "../helpers/equip_helper.h"
#include "../helpers/combat_helper.h"
#include "../helpers/grappling_helper.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include <math.h>
#include "../anim/ballchain/ballchain_anim_data.h"

// =============================================================================
// Internal Constants
// =============================================================================
#define WHIP_GRAPPLE_ACTOR_RADIUS   40.0f
#define WHIP_GRAPPLE_ACTOR_Y_OFFSET 30.0f
#define WHIP_MAX_HORIZ_SPEED        15.0f
#define WHIP_MAX_VERT_SPEED         12.0f
#define WHIP_YAW_TURN_RATE          256    // s16 units per frame at full stick (~1.4 deg/frame)

// =============================================================================
// Static Data
// =============================================================================
static u8 sWhipColInitialized = 0;

// =============================================================================
// Collider Functions
// =============================================================================
static void Whip_InitCollider(PlayState* play, Player* p) {
    if (sWhipColInitialized) return;
    Collider_InitCylinder(play, &whipCollider);
    Collider_SetCylinder(play, &whipCollider, &p->actor, &sWhipColInit);
    sWhipColInitialized = 1;
}

static void Whip_UpdateCollider(PlayState* play, Vec3f* pos) {
    whipCollider.dim.pos.x = (s16)pos->x;
    whipCollider.dim.pos.y = (s16)(pos->y - (WHIP_COL_HEIGHT / 2));
    whipCollider.dim.pos.z = (s16)pos->z;
    whipCollider.base.atFlags |= AT_ON | AT_TYPE_PLAYER;
    CollisionCheck_SetAT(play, &play->colChkCtx, &whipCollider.base);
}

// =============================================================================
// Table Lookup Functions
// =============================================================================
static s32 Whip_IsGrappleActor(Actor* actor) {
    s32 i;
    for (i = 0; i < (s32)WHIP_GRAPPLE_COUNT; i++) {
        if (actor->id == sWhipGrappleTable[i].actorId) {
            if (sWhipGrappleTable[i].params == -1 ||
                actor->params == sWhipGrappleTable[i].params) {
                return 1;
            }
        }
    }
    return 0;
}

static s32 Whip_IsParalyzeTarget(Actor* actor) {
    s32 i;
    for (i = 0; i < (s32)WHIP_PARALYZE_COUNT; i++) {
        if (actor->id == sWhipParalyzeTable[i].actorId) {
            if (sWhipParalyzeTable[i].params == -1 ||
                actor->params == sWhipParalyzeTable[i].params) {
                return 1;
            }
        }
    }
    return 0;
}

static s32 Whip_IsDisarmTarget(Actor* actor, WhipDisarmType* outType) {
    s32 i;
    for (i = 0; i < (s32)WHIP_DISARM_COUNT; i++) {
        if (actor->id == sWhipDisarmTable[i].actorId) {
            if (sWhipDisarmTable[i].params == -1 ||
                actor->params == sWhipDisarmTable[i].params) {
                if (outType != NULL) *outType = sWhipDisarmTable[i].type;
                return 1;
            }
        }
    }
    return 0;
}

// =============================================================================
// Enemy Interaction Functions
// =============================================================================
static void Whip_ApplyParalyze(Actor* enemy, Player* p, PlayState* play) {
    enemy->colorFilterTimer = WHIP_STUN_FRAMES;
    enemy->colorFilterParams = 0x0028; // blue tint
    enemy->speedXZ = 0.0f;
    whipPullTarget = enemy;
    whipState = WHIP_STATE_HIT_ENEMY;
    Audio_PlaySoundGeneral(WHIP_SFX_HIT_ENEMY, &enemy->world.pos, 4,
        &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

static void Whip_ApplyDisarm(Actor* enemy, WhipDisarmType type, PlayState* play) {
    enemy->home.rot.z |= WHIP_DISARMED_FLAG;
    Audio_PlaySoundGeneral(WHIP_SFX_DISARM, &enemy->world.pos, 4,
        &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

static void Whip_ApplyBoomerangDamage(Actor* enemy, Player* p, PlayState* play) {
    // AT collider already deals DMG_BOOMERANG via collision system
    // Set up rage mode countdown (activates after stun wears off)
    whipRageTarget = enemy;
    whipRageTimer = WHIP_RAGE_DURATION + WHIP_STUN_FRAMES;
    whipRageOrigSpeed = enemy->speedXZ;
    Audio_PlaySoundGeneral(WHIP_SFX_HIT_ENEMY, &enemy->world.pos, 4,
        &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

static void Whip_UpdateRage(PlayState* play) {
    if (whipRageTarget == NULL || whipRageTarget->update == NULL) {
        whipRageTarget = NULL;
        whipRageTimer = 0;
        return;
    }
    if (whipRageTimer > 0) {
        whipRageTimer--;
        // Rage activates after the initial stun wears off
        if (whipRageTimer <= WHIP_RAGE_DURATION && whipRageTimer > 0) {
            whipRageTarget->colorFilterTimer = 2;
            whipRageTarget->colorFilterParams = 0x4028; // red tint
            if (whipRageTarget->speedXZ > 0.1f) {
                whipRageTarget->speedXZ *= WHIP_RAGE_SPEED_MULT;
            }
        }
        if (whipRageTimer == 0) {
            whipRageTarget = NULL;
        }
    }
}

// =============================================================================
// Grapple Actor Proximity Check
// =============================================================================
static Actor* Whip_FindGrappleActor(PlayState* play, Vec3f* tipPos) {
    Actor* actor;
    Actor* next;
    f32 dist;
    s32 cat;

    for (cat = 0; cat < 2; cat++) {
        s32 category = (cat == 0) ? ACTORCAT_PROP : ACTORCAT_BG;
        for (actor = play->actorCtx.actorLists[category].head; actor != NULL; actor = next) {
            next = actor->next;
            if (actor->update == NULL) continue;
            if (!Whip_IsGrappleActor(actor)) continue;
            dist = Math_Vec3f_DistXYZ(tipPos, &actor->world.pos);
            if (dist < WHIP_GRAPPLE_ACTOR_RADIUS) {
                return actor;
            }
        }
    }
    return NULL;
}

// =============================================================================
// Stop / Start
// =============================================================================
static void Whip_Stop(Player* p, PlayState* play) {
    if (whipFirstPerson) {
        FirstPerson_Exit(p, play);
        whipFirstPerson = 0;
    }
    whipCollider.base.atFlags &= ~(AT_ON | AT_HIT);
    whipActive = 0;
    whipState = WHIP_STATE_INACTIVE;
    whipTimer = 0;
    whipPullTarget = NULL;
    whipSwingAngle = 0.0f;
    whipSwingVel = 0.0f;
    whipRopeLength = 0.0f;
    p->actor.gravity = -1.0f;
    ItemEquip_PlayUnequipSFX(play, p);
}

static void Whip_Start(Player* p, PlayState* play) {
    if (whipActive) return;
    whipActive = 1;
    whipState = WHIP_STATE_EQUIP;
    whipTimer = 0;
    whipPullTarget = NULL;
    whipRageTarget = NULL;
    whipRageTimer = 0;

    // When Z-targeting with focus actor, launch immediately toward target
    if (Player_IsZTargeting(p) && p->focusActor != NULL) {
        f32 dx = p->focusActor->focus.pos.x - p->actor.world.pos.x;
        f32 dy = p->focusActor->focus.pos.y - (p->actor.world.pos.y + 50.0f);
        f32 dz = p->focusActor->focus.pos.z - p->actor.world.pos.z;
        f32 hDist = sqrtf(dx * dx + dz * dz);

        whipExtendYaw = Math_Atan2S(dx, dz);
        whipExtendPitch = Math_Atan2S(-dy, hDist);
        whipTipPos = p->bodyPartsPos[PLAYER_BODYPART_R_HAND];
        whipTimer = WHIP_TIMER_MAX;
        whipState = WHIP_STATE_EXTENDING;

        p->actor.shape.rot.y = whipExtendYaw;
        p->actor.world.rot.y = whipExtendYaw;
        p->yaw = whipExtendYaw;

        whipFirstPerson = 0;
        Audio_PlaySoundGeneral(WHIP_SFX_THROW, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    } else {
        // Regular equip with first-person aiming
        if (!Player_IsZTargeting(p)) {
            FirstPerson_Init(p, play);
            whipFirstPerson = 1;
        } else {
            whipFirstPerson = 0;
        }
    }

    ItemEquip_PlayEquipSFX(play, p);
}

// =============================================================================
// State: Equip (coiled rope in hand, waiting for input)
// =============================================================================
static void WhipStateEquip(Player* p, PlayState* play, ItemInputState* in) {
    u8 isZTarget = Player_IsZTargeting(p);

    whipTipPos = p->bodyPartsPos[PLAYER_BODYPART_R_HAND];

    // Toggle first-person based on Z-targeting state
    if (whipFirstPerson && isZTarget) {
        FirstPerson_Exit(p, play);
        whipFirstPerson = 0;
    } else if (!whipFirstPerson && !isZTarget) {
        FirstPerson_Init(p, play);
        whipFirstPerson = 1;
    }

    // Update first-person camera each frame
    if (whipFirstPerson) {
        FirstPerson_Update(p, play);
    }

    if (in->isPressed) {
        // Get aim direction from first-person or Z-target
        if (whipFirstPerson) {
            whipExtendYaw = FirstPerson_GetAimYaw(p);
            whipExtendPitch = FirstPerson_GetAimPitch(p);
            FirstPerson_Exit(p, play);
            whipFirstPerson = 0;
        } else if (isZTarget && p->focusActor != NULL) {
            f32 dx = p->focusActor->focus.pos.x - p->actor.world.pos.x;
            f32 dy = p->focusActor->focus.pos.y - (p->actor.world.pos.y + 50.0f);
            f32 dz = p->focusActor->focus.pos.z - p->actor.world.pos.z;
            f32 hDist = sqrtf(dx * dx + dz * dz);
            whipExtendYaw = Math_Atan2S(dx, dz);
            whipExtendPitch = Math_Atan2S(-dy, hDist);
        } else {
            whipExtendYaw = p->actor.shape.rot.y;
            whipExtendPitch = 0;
        }

        whipTipPos = p->bodyPartsPos[PLAYER_BODYPART_R_HAND];
        whipTimer = WHIP_TIMER_MAX;
        whipState = WHIP_STATE_EXTENDING;

        p->actor.shape.rot.y = whipExtendYaw;
        p->actor.world.rot.y = whipExtendYaw;
        p->yaw = whipExtendYaw;

        Audio_PlaySoundGeneral(WHIP_SFX_THROW, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }
}

// =============================================================================
// State: Extending (tip traveling forward)
// =============================================================================
static void WhipStateExtending(Player* p, PlayState* play) {
    Vec3f prevTip;
    Vec3f hitPos;
    CollisionPoly* hitPoly = NULL;
    s32 bgId = BGCHECK_SCENE;
    GrappleTarget target;
    Actor* grappleActor;
    f32 cosP, sinP, cosY, sinY;

    p->actor.speedXZ = 0.0f;
    p->linearVelocity = 0.0f;
    p->skelAnime.playSpeed = 0.0f;
    p->actor.shape.rot.y = whipExtendYaw;
    p->actor.world.rot.y = whipExtendYaw;
    p->yaw = whipExtendYaw;

    // Save previous tip for line test
    prevTip = whipTipPos;

    // Advance tip along aim direction
    cosP = Math_CosS(whipExtendPitch);
    sinP = Math_SinS(whipExtendPitch);
    cosY = Math_CosS(whipExtendYaw);
    sinY = Math_SinS(whipExtendYaw);

    whipTipPos.x += sinY * cosP * WHIP_EXTEND_SPEED;
    whipTipPos.y -= sinP * WHIP_EXTEND_SPEED;
    whipTipPos.z += cosY * cosP * WHIP_EXTEND_SPEED;

    // Update collider at new tip position
    Whip_UpdateCollider(play, &whipTipPos);

    // Check 1: Surface collision — only attach if beam/bar shaped (graspable)
    if (BgCheck_EntityLineTest1(&play->colCtx, &prevTip, &whipTipPos, &hitPos,
                                 &hitPoly, true, true, true, true, &bgId)) {
        whipTipPos = hitPos;
        Grapple_AnalyzeSurface(play, hitPoly, bgId, &hitPos, &target);

        if (target.isGraspable) {
            // Beam/bar shaped surface — attach and swing
            whipAttachPos = hitPos;
            whipAttachNormal = target.surfaceNormal;
            whipAttachedBgId = bgId;
            whipState = WHIP_STATE_ATTACHED;
            Audio_PlaySoundGeneral(WHIP_SFX_HIT_SURFACE, &hitPos, 4,
                &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            return;
        }

        // Not graspable (flat wall, floor, etc.) — retract
        whipState = WHIP_STATE_RETRACTING;
        return;
    }

    // Check 2: Graspable actor proximity
    grappleActor = Whip_FindGrappleActor(play, &whipTipPos);
    if (grappleActor != NULL) {
        whipAttachPos = grappleActor->world.pos;
        whipAttachPos.y += WHIP_GRAPPLE_ACTOR_Y_OFFSET;
        whipAttachNormal.x = 0.0f;
        whipAttachNormal.y = 1.0f;
        whipAttachNormal.z = 0.0f;
        whipAttachedBgId = BGCHECK_SCENE;
        whipState = WHIP_STATE_ATTACHED;
        Audio_PlaySoundGeneral(WHIP_SFX_HIT_SURFACE, &grappleActor->world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        return;
    }

    // Check 3: Enemy hit via collider AT
    if (whipCollider.base.atFlags & AT_HIT) {
        Actor* hitActor = whipCollider.base.at;
        whipCollider.base.atFlags &= ~AT_HIT;

        if (hitActor != NULL && hitActor->update != NULL) {
            WhipDisarmType disarmType;

            if (Whip_IsParalyzeTarget(hitActor)) {
                Whip_ApplyParalyze(hitActor, p, play);
                return;
            }
            if (Whip_IsDisarmTarget(hitActor, &disarmType)) {
                Whip_ApplyDisarm(hitActor, disarmType, play);
                whipState = WHIP_STATE_RETRACTING;
                return;
            }
            Whip_ApplyBoomerangDamage(hitActor, p, play);
            whipState = WHIP_STATE_RETRACTING;
            return;
        }
    }

    // Check 4: Timer expired
    whipTimer--;
    if (whipTimer <= 0) {
        whipState = WHIP_STATE_RETRACTING;
    }
}

// =============================================================================
// State: HitEnemy (paralyze + pull toward Link)
// =============================================================================
static void WhipStateHitEnemy(Player* p, PlayState* play) {
    f32 dx, dy, dz, dist, norm;

    p->actor.speedXZ = 0.0f;
    p->linearVelocity = 0.0f;
    p->skelAnime.playSpeed = 0.0f;

    if (whipPullTarget == NULL || whipPullTarget->update == NULL) {
        whipPullTarget = NULL;
        whipState = WHIP_STATE_RETRACTING;
        return;
    }

    whipTipPos = whipPullTarget->world.pos;

    dx = p->actor.world.pos.x - whipPullTarget->world.pos.x;
    dy = (p->actor.world.pos.y + 30.0f) - whipPullTarget->world.pos.y;
    dz = p->actor.world.pos.z - whipPullTarget->world.pos.z;
    dist = sqrtf(dx * dx + dy * dy + dz * dz);

    if (dist < WHIP_PULL_ARRIVE_DIST) {
        whipPullTarget = NULL;
        whipState = WHIP_STATE_RETRACTING;
        return;
    }

    if (dist > 0.1f) {
        norm = WHIP_PULL_SPEED / dist;
        whipPullTarget->world.pos.x += dx * norm;
        whipPullTarget->world.pos.y += dy * norm;
        whipPullTarget->world.pos.z += dz * norm;
    }

    whipPullTarget->speedXZ = 0.0f;
    func_8002F974(&p->actor, WHIP_SFX_SWING);
}

// =============================================================================
// State: Attached (setup swing parameters, immediate transition to SWINGING)
// =============================================================================
static void WhipStateAttached(Player* p, PlayState* play) {
    f32 dx, dz, hDist, vDist;

    // Fixed rope length: always 2 adult Links tall
    whipRopeLength = WHIP_FIXED_ROPE_LENGTH;

    dx = p->actor.world.pos.x - whipAttachPos.x;
    dz = p->actor.world.pos.z - whipAttachPos.z;
    whipSwingYaw = Math_Atan2S(dx, dz);

    hDist = sqrtf(dx * dx + dz * dz);
    vDist = whipAttachPos.y - p->actor.world.pos.y;

    if (vDist > 0.1f) {
        whipSwingAngle = atan2f(hDist, vDist);
    } else {
        whipSwingAngle = WHIP_MAX_ANGLE * 0.5f;
    }

    whipSwingVel = 0.0f;
    whipState = WHIP_STATE_SWINGING;
}

// =============================================================================
// State: Swinging (pendulum physics)
// =============================================================================
static void WhipStateSwinging(Player* p, PlayState* play, ItemInputState* in) {
    f32 angAccel, stickInputX, stickInputY;
    f32 sinA, cosA, swingDirX, swingDirZ;

    // Disable normal player physics
    p->actor.gravity = 0.0f;
    p->actor.velocity.y = 0.0f;
    p->actor.speedXZ = 0.0f;
    p->linearVelocity = 0.0f;
    p->skelAnime.playSpeed = 0.0f;

    // Apply ball chain spin pose (arms raised, holding whip)
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

    // Pendulum: angular acceleration from gravity
    angAccel = -WHIP_GRAVITY * sinf(whipSwingAngle);

    // Stick Y input: lean forward/backward for swing momentum
    stickInputY = (f32)play->state.input[0].cur.stick_y / 127.0f;
    angAccel += stickInputY * WHIP_INPUT_FORCE;

    // Stick X input: slightly turn the swing plane (yaw), keeping momentum
    stickInputX = (f32)play->state.input[0].cur.stick_x / 127.0f;
    whipSwingYaw += (s16)(stickInputX * WHIP_YAW_TURN_RATE);

    // Integrate
    whipSwingVel = (whipSwingVel + angAccel) * WHIP_DAMPING;
    whipSwingAngle += whipSwingVel;

    // Clamp
    if (whipSwingAngle > WHIP_MAX_ANGLE) {
        whipSwingAngle = WHIP_MAX_ANGLE;
        whipSwingVel = 0.0f;
    }
    if (whipSwingAngle < -WHIP_MAX_ANGLE) {
        whipSwingAngle = -WHIP_MAX_ANGLE;
        whipSwingVel = 0.0f;
    }

    // Calculate player position from pendulum
    sinA = sinf(whipSwingAngle);
    cosA = cosf(whipSwingAngle);
    swingDirX = Math_SinS(whipSwingYaw);
    swingDirZ = Math_CosS(whipSwingYaw);

    p->actor.world.pos.x = whipAttachPos.x + sinA * swingDirX * whipRopeLength;
    p->actor.world.pos.y = whipAttachPos.y - cosA * whipRopeLength;
    p->actor.world.pos.z = whipAttachPos.z + sinA * swingDirZ * whipRopeLength;

    // Tip at attach point (for rope rendering)
    whipTipPos = whipAttachPos;

    // Face swing direction
    if (whipSwingVel > 0.001f) {
        p->actor.shape.rot.y = whipSwingYaw;
    } else if (whipSwingVel < -0.001f) {
        p->actor.shape.rot.y = whipSwingYaw + 0x8000;
    }

    func_8002F974(&p->actor, WHIP_SFX_SWING);

    // Ground contact check: if Link touches the floor, unequip
    if (p->actor.world.pos.y <= p->actor.floorHeight + 5.0f) {
        p->actor.world.pos.y = p->actor.floorHeight;
        p->actor.gravity = -1.0f;
        Whip_Stop(p, play);
        return;
    }

    // C-button press: release with momentum jump
    if (in->isPressed) {
        f32 omega = whipSwingVel;
        f32 cosTheta = cosf(whipSwingAngle);
        f32 sinTheta = sinf(whipSwingAngle);
        f32 horizSpeed, vertSpeed;

        // Tangential velocity decomposed into horizontal and vertical
        horizSpeed = cosTheta * omega * whipRopeLength * WHIP_RELEASE_BOOST;
        vertSpeed = sinTheta * omega * whipRopeLength * WHIP_RELEASE_BOOST;

        // Horizontal momentum
        p->actor.speedXZ = fabsf(horizSpeed);
        if (p->actor.speedXZ > WHIP_MAX_HORIZ_SPEED) {
            p->actor.speedXZ = WHIP_MAX_HORIZ_SPEED;
        }

        // Face release direction
        if (horizSpeed >= 0.0f) {
            p->actor.shape.rot.y = whipSwingYaw;
        } else {
            p->actor.shape.rot.y = whipSwingYaw + 0x8000;
        }
        p->actor.world.rot.y = p->actor.shape.rot.y;
        p->yaw = p->actor.shape.rot.y;

        // Vertical launch (minimum upward boost to feel like a jump)
        p->actor.velocity.y = vertSpeed;
        if (p->actor.velocity.y < 4.0f) p->actor.velocity.y = 4.0f;
        if (p->actor.velocity.y > WHIP_MAX_VERT_SPEED) p->actor.velocity.y = WHIP_MAX_VERT_SPEED;

        p->actor.gravity = -1.0f;
        Whip_Stop(p, play);
        return;
    }
}

// =============================================================================
// State: Retracting (rope returning to Link)
// =============================================================================
static void WhipStateRetracting(Player* p, PlayState* play) {
    Vec3f handPos;
    f32 dx, dy, dz, dist, norm;

    handPos = p->bodyPartsPos[PLAYER_BODYPART_R_HAND];

    dx = handPos.x - whipTipPos.x;
    dy = handPos.y - whipTipPos.y;
    dz = handPos.z - whipTipPos.z;
    dist = sqrtf(dx * dx + dy * dy + dz * dz);

    if (dist < WHIP_ARRIVE_DIST) {
        whipTipPos = handPos;
        whipState = WHIP_STATE_EQUIP;
        Audio_PlaySoundGeneral(WHIP_SFX_RETRACT, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        return;
    }

    if (dist > 0.1f) {
        norm = WHIP_RETRACT_SPEED / dist;
        whipTipPos.x += dx * norm;
        whipTipPos.y += dy * norm;
        whipTipPos.z += dz * norm;
    }

    func_8002F974(&p->actor, WHIP_SFX_SWING);
}

// =============================================================================
// Public API
// =============================================================================
void Handle_Whip(Player* p, PlayState* play) {
    ItemInputState in;

    if (!sWhipColInitialized) {
        Whip_InitCollider(play, p);
    }

    // Rage mode runs independently of whip state
    Whip_UpdateRage(play);

    ItemInput_Update(&in, ITEM_WHIP, p, play);

    if (!in.wasEquipped || ItemInput_IsBlocked(p, play) || ItemInput_CheckDamage(p, &whipPrevInvinc)) {
        if (whipActive) Whip_Stop(p, play);
        return;
    }
    if (in.otherButtonPressed) {
        if (whipActive) Whip_Stop(p, play);
        return;
    }

    if (!whipActive) {
        if (in.isPressed || in.isHeld) {
            Whip_Start(p, play);
        }
        return;
    }

    switch (whipState) {
        case WHIP_STATE_EQUIP:      WhipStateEquip(p, play, &in);    break;
        case WHIP_STATE_EXTENDING:  WhipStateExtending(p, play);      break;
        case WHIP_STATE_HIT_ENEMY:  WhipStateHitEnemy(p, play);       break;
        case WHIP_STATE_ATTACHED:   WhipStateAttached(p, play);       break;
        case WHIP_STATE_SWINGING:   WhipStateSwinging(p, play, &in);  break;
        case WHIP_STATE_RETRACTING: WhipStateRetracting(p, play);     break;
        default:                    whipState = WHIP_STATE_EQUIP; break;
    }
}

void Player_InitWhipIA(PlayState* play, Player* p) {
    Whip_InitCollider(play, p);
    whipActive = 0;
    whipState = WHIP_STATE_INACTIVE;
    whipTimer = 0;
    whipPullTarget = NULL;
    whipRageTarget = NULL;
    whipRageTimer = 0;
    whipSwingAngle = 0.0f;
    whipSwingVel = 0.0f;
    whipRopeLength = 0.0f;
    whipFirstPerson = 0;
}

s32 Player_UpperAction_Whip(Player* p, PlayState* play) {
    return 0;
}

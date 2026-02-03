/**
 * item_dominionrod.c - Dominion Rod from Twilight Princess
 *
 * Controls:
 *   C Button:            Fire golden orb projectile
 *   C Button (possessed): Actor special ability
 *   Analog (possessed):   Move possessed actor
 *   A Button (possessed): Jump (if supported)
 *
 * Supported Actors:
 *   - Beamos: Mimics Link + jumps, C-button = laser
 *   - Armos: Mimics Link + jumps, C-button = explode
 *   - Anubis: Mimics Link (including Y axis), C-button = fire
 */

#include "z64.h"
#include "item_dominionrod.h"
#include "../custom_items.h"
#include "../helpers/camera_helper.h"
#include "../helpers/equip_helper.h"
#include "../helpers/combat_helper.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "objects/object_fhg/object_fhg.h"
#include "overlays/effects/ovl_Effect_Ss_Fhg_Flash/z_eff_ss_fhg_flash.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"

// ============================================================================
// STATIC STATE
// ============================================================================
static u8 sDomRodColInitialized = 0;
static u8 sControlFirstFrame = 0;
static Vec3f sLastLinkPos = { 0, 0, 0 };
static u8 sLinkWasJumping = 0;

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

static void DomRod_InitCollider(PlayState* play, Player* p) {
    if (sDomRodColInitialized) return;
    Collider_InitCylinder(play, &domRodCollider);
    Collider_SetCylinder(play, &domRodCollider, &p->actor, &sDomRodColliderInit);
    sDomRodColInitialized = 1;
}

static void DomRod_UpdateCollider(PlayState* play, Vec3f* pos) {
    domRodCollider.dim.pos.x = (s16)pos->x;
    domRodCollider.dim.pos.y = (s16)pos->y;
    domRodCollider.dim.pos.z = (s16)pos->z;
    domRodCollider.base.atFlags |= AT_ON | AT_TYPE_PLAYER;
    CollisionCheck_SetAT(play, &play->colChkCtx, &domRodCollider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &domRodCollider.base);
}

static void DomRod_PlaySound(Vec3f* pos, u16 sfxId) {
    Audio_PlaySoundGeneral(sfxId, pos, 4,
        &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

static void DomRod_PlayLoopSound(Actor* actor, u16 sfxId) {
    func_8002F974(actor, sfxId - SFX_FLAG);
}

// ============================================================================
// LIGHT SOURCE MANAGEMENT
// ============================================================================

static void DomRod_CreateLight(PlayState* play) {
    domRodLightNode = LightContext_InsertLight(play, &play->lightCtx, &domRodLightInfo);
    Lights_PointNoGlowSetInfo(&domRodLightInfo,
        (s16)domRodOrbPos.x, (s16)domRodOrbPos.y, (s16)domRodOrbPos.z,
        DOMROD_ORB_ENV_R, DOMROD_ORB_ENV_G, DOMROD_ORB_ENV_B, DOMROD_ORB_LIGHT_RADIUS);
}

static void DomRod_UpdateLight(void) {
    if (domRodLightNode != NULL) {
        Lights_PointNoGlowSetInfo(&domRodLightInfo,
            (s16)domRodOrbPos.x, (s16)domRodOrbPos.y, (s16)domRodOrbPos.z,
            DOMROD_ORB_ENV_R, DOMROD_ORB_ENV_G, DOMROD_ORB_ENV_B, DOMROD_ORB_LIGHT_RADIUS);
    }
}

static void DomRod_RemoveLight(PlayState* play) {
    if (domRodLightNode != NULL) {
        LightContext_RemoveLight(play, &play->lightCtx, domRodLightNode);
        domRodLightNode = NULL;
    }
}

// ============================================================================
// CONTROLLABLE ACTOR DETECTION (ONLY 3 ACTORS)
// ============================================================================

static u8 DomRod_IsActorControllable(Actor* actor) {
    if (actor == NULL || actor->update == NULL) return DOMROD_CONTROL_NONE;

    switch (actor->id) {
        case ACTOR_EN_VM:      // Beamos - mimics Link + jumps
            return DOMROD_CONTROL_MOVEMENT;
        case ACTOR_EN_AM:      // Armos - mimics Link + jumps
            return DOMROD_CONTROL_MOVEMENT;
        case ACTOR_EN_ANUBICE: // Anubis - mimics Link (floats, Y axis too)
            return DOMROD_CONTROL_MOVEMENT;
        default:
            return DOMROD_CONTROL_NONE;
    }
}

// ============================================================================
// STATE TRANSITION FUNCTIONS
// ============================================================================

static void DomRod_Stop(Player* p, PlayState* play) {
    if (domRodFirstPerson) {
        FirstPerson_Exit(p, play);
        domRodFirstPerson = 0;
    }

    DomRod_RemoveLight(play);

    domRodCollider.base.atFlags &= ~(AT_ON | AT_HIT);
    domRodActive = 0;
    domRodState = DOMROD_STATE_IDLE;
    domRodControlledActor = NULL;
    domRodControlType = DOMROD_CONTROL_NONE;
    domRodDamagePaused = 0;
    domRodAttackCooldown = 0;
    domRodCButtonHoldTimer = 0;

    ItemEquip_PlayUnequipSFX(play, p);
}

static void DomRod_Start(Player* p, PlayState* play) {
    if (domRodActive) return;

    domRodActive = 1;
    domRodState = DOMROD_STATE_AIMING;
    domRodFirstPerson = 1;
    domRodControlledActor = NULL;
    domRodTimer = DOMROD_ORB_MAX_TIME;
    domRodControlType = DOMROD_CONTROL_NONE;
    domRodDamagePaused = 0;
    domRodAttackCooldown = 0;
    domRodCButtonHoldTimer = 0;

    FirstPerson_Init(p, play);
    ItemEquip_PlayEquipSFX(play, p);
}

static void DomRod_Launch(Player* p, PlayState* play) {
    domRodState = DOMROD_STATE_ORB_FLYING;
    domRodTimer = DOMROD_ORB_MAX_TIME;
    domRodStartPos = p->actor.world.pos;

    s16 launchYaw = FirstPerson_GetAimYaw(p);
    s16 launchPitch = FirstPerson_GetAimPitch(p);

    domRodOrbPos.x = p->actor.world.pos.x + Math_SinS(launchYaw) * DOMROD_ORB_LAUNCH_OFFSET_XZ;
    domRodOrbPos.y = p->actor.world.pos.y + DOMROD_ORB_LAUNCH_OFFSET_Y;
    domRodOrbPos.z = p->actor.world.pos.z + Math_CosS(launchYaw) * DOMROD_ORB_LAUNCH_OFFSET_XZ;

    domRodOrbRot.x = launchPitch;
    domRodOrbRot.y = launchYaw;
    domRodOrbRot.z = 0;

    FirstPerson_Exit(p, play);
    domRodFirstPerson = 0;

    DomRod_CreateLight(play);
    DomRod_PlaySound(&p->actor.world.pos, DOMROD_SFX_LAUNCH);
}

static void DomRod_StartReturn(Player* p, PlayState* play) {
    domRodState = DOMROD_STATE_ORB_RETURN;
    domRodControlledActor = NULL;
    domRodControlType = DOMROD_CONTROL_NONE;
    domRodCButtonHoldTimer = 0;
    DomRod_PlaySound(&domRodOrbPos, DOMROD_SFX_HIT_WALL);
}

static void DomRod_StartControl(Player* p, PlayState* play, Actor* target) {
    domRodState = DOMROD_STATE_CONTROLLING;
    domRodControlledActor = target;
    domRodControlType = DomRod_IsActorControllable(target);
    domRodControlVel.x = 0;
    domRodControlVel.y = 0;
    domRodControlVel.z = 0;
    domRodAttackCooldown = 0;
    domRodCButtonHoldTimer = 0;
    sControlFirstFrame = 1;

    // Store Link's current position for delta tracking
    sLastLinkPos = p->actor.world.pos;
    sLinkWasJumping = 0;

    domRodOrbPos = target->focus.pos;

    DomRod_PlaySound(&domRodOrbPos, DOMROD_SFX_POSSESS);
}

static void DomRod_EndControl(Player* p, PlayState* play) {
    DomRod_PlaySound(&domRodOrbPos, DOMROD_SFX_RELEASE);
    domRodCButtonHoldTimer = 0;
    DomRod_StartReturn(p, play);
}

// ============================================================================
// ORB MOVEMENT
// ============================================================================

static void DomRod_MoveOrb(f32 speed) {
    f32 cosP = Math_CosS(domRodOrbRot.x);
    f32 sinP = Math_SinS(domRodOrbRot.x);
    f32 sinY = Math_SinS(domRodOrbRot.y);
    f32 cosY = Math_CosS(domRodOrbRot.y);

    domRodOrbPos.x += sinY * cosP * speed;
    domRodOrbPos.y -= sinP * speed;
    domRodOrbPos.z += cosY * cosP * speed;
}

// ============================================================================
// COLLISION CHECKS
// ============================================================================

static Actor* DomRod_FindNearbyControllableActor(PlayState* play) {
    Actor* closestActor = NULL;
    f32 closestDist = DOMROD_ORB_COLLIDER_RADIUS + 40.0f;

    for (s32 category = 0; category < ACTORCAT_MAX; category++) {
        Actor* actor = play->actorCtx.actorLists[category].head;
        while (actor != NULL) {
            if (actor->update != NULL && DomRod_IsActorControllable(actor) != DOMROD_CONTROL_NONE) {
                f32 dist = Math_Vec3f_DistXYZ(&domRodOrbPos, &actor->world.pos);
                if (dist < closestDist) {
                    closestDist = dist;
                    closestActor = actor;
                }
            }
            actor = actor->next;
        }
    }

    return closestActor;
}

static Actor* DomRod_CheckActorHit(Player* p, PlayState* play) {
    if (domRodCollider.base.atFlags & AT_HIT) {
        Actor* hitActor = domRodCollider.base.at;
        domRodCollider.base.atFlags &= ~AT_HIT;

        if (hitActor != NULL && DomRod_IsActorControllable(hitActor) != DOMROD_CONTROL_NONE) {
            return hitActor;
        }
    }

    return DomRod_FindNearbyControllableActor(play);
}

static u8 DomRod_CheckGeometryHit(PlayState* play) {
    Vec3f hitPoint;
    CollisionPoly* hitPoly = NULL;
    s32 hitDynaId = 0;

    f32 cosP = Math_CosS(domRodOrbRot.x);
    f32 sinP = Math_SinS(domRodOrbRot.x);
    f32 sinY = Math_SinS(domRodOrbRot.y);
    f32 cosY = Math_CosS(domRodOrbRot.y);

    Vec3f prevPos = domRodOrbPos;
    prevPos.x -= sinY * cosP * DOMROD_ORB_SPEED;
    prevPos.y += sinP * DOMROD_ORB_SPEED;
    prevPos.z -= cosY * cosP * DOMROD_ORB_SPEED;

    if (BgCheck_EntityLineTest1(&play->colCtx, &prevPos, &domRodOrbPos, &hitPoint,
            &hitPoly, true, true, true, true, &hitDynaId)) {
        domRodOrbPos = hitPoint;
        DomRod_PlaySound(&domRodOrbPos, DOMROD_SFX_HIT_WALL);
        return 1;
    }
    return 0;
}

// ============================================================================
// LINK MOVEMENT MIMIC HELPER
// ============================================================================

static u8 DomRod_IsLinkJumping(Player* p) {
    // Check if Link is in any jumping/airborne state
    return !(p->actor.bgCheckFlags & BGCHECKFLAG_GROUND);
}

static void DomRod_MimicLinkMovement(Player* p, PlayState* play, Actor* actor, u8 includeYAxis, u8 canJump) {
    // Calculate Link's movement delta this frame
    Vec3f linkDelta;
    linkDelta.x = p->actor.world.pos.x - sLastLinkPos.x;
    linkDelta.y = p->actor.world.pos.y - sLastLinkPos.y;
    linkDelta.z = p->actor.world.pos.z - sLastLinkPos.z;

    // Apply delta to actor position (mimic Link)
    actor->world.pos.x += linkDelta.x;
    actor->world.pos.z += linkDelta.z;

    // Y axis only for floating actors (Anubis)
    if (includeYAxis) {
        actor->world.pos.y += linkDelta.y;
    }

    // Jump detection for Beamos and Armos
    if (canJump) {
        u8 linkIsJumping = DomRod_IsLinkJumping(p);

        // Link just started jumping - make actor jump too
        if (linkIsJumping && !sLinkWasJumping && (actor->bgCheckFlags & BGCHECKFLAG_GROUND)) {
            actor->velocity.y = DOMROD_ARMOS_HOP_VEL_Y;
            DomRod_PlaySound(&actor->world.pos, NA_SE_EN_DODO_M_GND);
        }

        sLinkWasJumping = linkIsJumping;
    }

    // Make actor face same direction as Link
    actor->shape.rot.y = p->actor.shape.rot.y;
    actor->world.rot.y = p->actor.world.rot.y;

    // Update last position for next frame
    sLastLinkPos = p->actor.world.pos;
}

// ============================================================================
// ACTOR-SPECIFIC CONTROL: BEAMOS
// Mimics Link movement + jumps, C-button = laser
// ============================================================================

static void DomRod_ControlBeamos(Player* p, PlayState* play, Actor* actor) {
    Input* input = &play->state.input[0];

    // DISABLE AI: Force all movement values to 0
    actor->speedXZ = 0;
    actor->velocity.x = 0;
    actor->velocity.z = 0;
    // Don't zero velocity.y so gravity/jumps work

    // Mimic Link's movement (with jumps)
    DomRod_MimicLinkMovement(p, play, actor, 0, 1);

    // C-button = Fire laser (second press after possession)
    if (CHECK_BTN_ALL(input->press.button, domRodButtonMask) && domRodAttackCooldown == 0) {
        // Fire laser sound
        DomRod_PlaySound(&actor->world.pos, NA_SE_EN_VALVAISA_FIRE);
        domRodAttackCooldown = DOMROD_BEAMOS_LASER_COOLDOWN;
    }

    if (domRodAttackCooldown > 0) domRodAttackCooldown--;
    domRodOrbPos = actor->focus.pos;
}

// ============================================================================
// ACTOR-SPECIFIC CONTROL: ARMOS
// Mimics Link movement + jumps, C-button = explode
// ============================================================================

static void DomRod_ControlArmos(Player* p, PlayState* play, Actor* actor) {
    Input* input = &play->state.input[0];

    // DISABLE AI: Force all movement values to 0
    actor->speedXZ = 0;
    actor->velocity.x = 0;
    actor->velocity.z = 0;
    // Don't zero velocity.y so gravity/jumps work

    // Mimic Link's movement (with jumps)
    DomRod_MimicLinkMovement(p, play, actor, 0, 1);

    // C-button = Self-destruct (explode)
    if (CHECK_BTN_ALL(input->press.button, domRodButtonMask)) {
        Actor* bomb = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM,
            actor->world.pos.x, actor->world.pos.y, actor->world.pos.z,
            0, 0, 0x6FF, BOMB_BODY, true);
        if (bomb != NULL) {
            EnBom* bombActor = (EnBom*)bomb;
            bombActor->timer = 0;
        }
        Actor_Kill(actor);
        DomRod_PlaySound(&actor->world.pos, DOMROD_SFX_EXPLODE);
        DomRod_EndControl(p, play);
        return;
    }

    domRodOrbPos = actor->focus.pos;
}

// ============================================================================
// ACTOR-SPECIFIC CONTROL: ANUBIS
// Mimics Link movement (including Y axis - floats), C-button = fire
// ============================================================================

static void DomRod_ControlAnubis(Player* p, PlayState* play, Actor* actor) {
    Input* input = &play->state.input[0];

    // DISABLE AI: Force all movement/velocity to 0
    actor->speedXZ = 0;
    actor->velocity.x = 0;
    actor->velocity.y = 0;
    actor->velocity.z = 0;

    // Mimic Link's movement (including Y axis for floating, no jumps)
    DomRod_MimicLinkMovement(p, play, actor, 1, 0);

    // C-button = Fire projectile
    if (CHECK_BTN_ALL(input->press.button, domRodButtonMask) && domRodAttackCooldown == 0) {
        // Spawn fireball in front of Anubis
        s16 fireYaw = actor->shape.rot.y;
        Vec3f firePos;
        firePos.x = actor->world.pos.x + Math_SinS(fireYaw) * 30.0f;
        firePos.y = actor->world.pos.y + 30.0f;
        firePos.z = actor->world.pos.z + Math_CosS(fireYaw) * 30.0f;

        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ANUBICE_FIRE,
            firePos.x, firePos.y, firePos.z,
            0, fireYaw, 0, 0, true);
        DomRod_PlaySound(&actor->world.pos, NA_SE_EN_ANUBIS_FIRE);
        domRodAttackCooldown = DOMROD_ANUBIS_FIRE_COOLDOWN;
    }

    if (domRodAttackCooldown > 0) domRodAttackCooldown--;
    domRodOrbPos = actor->focus.pos;
}

// ============================================================================
// PARTICLE EFFECTS
// ============================================================================

static void DomRod_SpawnOrbParticles(PlayState* play) {
    Vec3f vel = { 0, 0, 0 };
    Vec3f accel = { 0, -0.08f, 0 };

    Vec3f particlePos = domRodOrbPos;
    particlePos.x += Rand_CenteredFloat(15.0f);
    particlePos.y += Rand_CenteredFloat(15.0f);
    particlePos.z += Rand_CenteredFloat(15.0f);

    EffectSsFhgFlash_SpawnLightBall(play, &particlePos, &vel, &accel,
        (s16)(Rand_ZeroOne() * 60.0f) + 100, FHGFLASH_LIGHTBALL_GREEN);
}

// ============================================================================
// STATE UPDATE FUNCTIONS
// ============================================================================

static void DomRod_StateAiming(Player* p, PlayState* play, ItemInputState* in) {
    if (domRodFirstPerson) {
        FirstPerson_Update(p, play);
    }

    u8 isZTargeting = Player_IsZTargeting(p);
    if (domRodFirstPerson && isZTargeting) {
        FirstPerson_Exit(p, play);
        domRodFirstPerson = 0;
    } else if (!domRodFirstPerson && !isZTargeting) {
        FirstPerson_Init(p, play);
        domRodFirstPerson = 1;
    }

    if (!in->isHeld && !in->isPressed) {
        DomRod_Launch(p, play);
    }
}

static void DomRod_StateOrbFlying(Player* p, PlayState* play) {
    Actor* hitActor = DomRod_CheckActorHit(p, play);
    if (hitActor != NULL) {
        DomRod_StartControl(p, play, hitActor);
        return;
    }

    f32 orbSpeed = DOMROD_ORB_SPEED;
    if (Player_IsZTargeting(p) && p->focusActor != NULL && p->focusActor->update != NULL) {
        s16 targetYaw = Math_Vec3f_Yaw(&domRodOrbPos, &p->focusActor->focus.pos);
        s16 targetPitch = Math_Vec3f_Pitch(&domRodOrbPos, &p->focusActor->focus.pos);

        Math_ApproachS(&domRodOrbRot.y, targetYaw, 1, DOMROD_HOMING_TURN_SPEED);
        Math_ApproachS(&domRodOrbRot.x, targetPitch, 1, DOMROD_HOMING_TURN_SPEED);

        orbSpeed = DOMROD_ORB_SPEED_HOMING;
    }

    DomRod_MoveOrb(orbSpeed);
    DomRod_UpdateCollider(play, &domRodOrbPos);
    DomRod_UpdateLight();

    if ((play->gameplayFrames % 3) == 0) {
        DomRod_SpawnOrbParticles(play);
    }

    if (DomRod_CheckGeometryHit(play)) {
        DomRod_StartReturn(p, play);
        return;
    }

    f32 distFromStart = Math_Vec3f_DistXYZ(&domRodOrbPos, &domRodStartPos);
    if (distFromStart > DOMROD_ORB_MAX_RANGE || DECR(domRodTimer) == 0) {
        DomRod_StartReturn(p, play);
        return;
    }

    DomRod_PlayLoopSound(&p->actor, DOMROD_SFX_FLY);
}

static void DomRod_StateOrbReturning(Player* p, PlayState* play) {
    Vec3f targetPos = p->actor.world.pos;
    targetPos.y += DOMROD_ORB_LAUNCH_OFFSET_Y;

    f32 distToLink = Math_Vec3f_DistXYZ(&domRodOrbPos, &targetPos);

    if (distToLink > DOMROD_ORB_CATCH_DISTANCE) {
        f32 dx = targetPos.x - domRodOrbPos.x;
        f32 dy = targetPos.y - domRodOrbPos.y;
        f32 dz = targetPos.z - domRodOrbPos.z;

        if (distToLink > 0.1f) {
            f32 invNorm = DOMROD_ORB_RETURN_SPEED / distToLink;
            domRodOrbPos.x += dx * invNorm;
            domRodOrbPos.y += dy * invNorm;
            domRodOrbPos.z += dz * invNorm;
        }

        domRodOrbRot.y = Math_Vec3f_Yaw(&domRodOrbPos, &targetPos);
        domRodOrbRot.x = Math_Vec3f_Pitch(&domRodOrbPos, &targetPos);

        DomRod_UpdateLight();
        DomRod_PlayLoopSound(&p->actor, DOMROD_SFX_FLY);

        if ((play->gameplayFrames % 4) == 0) {
            DomRod_SpawnOrbParticles(play);
        }
    } else {
        domRodState = DOMROD_STATE_CATCHING;
        domRodTimer = DOMROD_CATCH_ANIM_FRAMES;
        DomRod_PlaySound(&p->actor.world.pos, DOMROD_SFX_CATCH);
    }
}

static void DomRod_StateControlling(Player* p, PlayState* play, ItemInputState* in) {
    // Check if actor is still valid
    if (domRodControlledActor == NULL || domRodControlledActor->update == NULL) {
        DomRod_EndControl(p, play);
        return;
    }

    // Distance check
    f32 distToActor = Math_Vec3f_DistXYZ(&p->actor.world.pos, &domRodControlledActor->world.pos);
    if (distToActor > DOMROD_CONTROL_MAX_DISTANCE) {
        DomRod_EndControl(p, play);
        return;
    }

    // B button or other C-button ends control
    if (in->otherButtonPressed || CHECK_BTN_ALL(play->state.input[0].press.button, BTN_B)) {
        DomRod_EndControl(p, play);
        return;
    }

    // Damage pauses control but doesn't end it
    if (p->invincibilityTimer > 0 && !domRodDamagePaused) {
        domRodDamagePaused = 1;
    }
    if (domRodDamagePaused && p->invincibilityTimer == 0) {
        domRodDamagePaused = 0;
    }

    if (!domRodDamagePaused) {
        // Route to actor-specific control handler (only 3 actors)
        switch (domRodControlledActor->id) {
            case ACTOR_EN_VM:      // Beamos
                DomRod_ControlBeamos(p, play, domRodControlledActor);
                break;
            case ACTOR_EN_AM:      // Armos
                DomRod_ControlArmos(p, play, domRodControlledActor);
                break;
            case ACTOR_EN_ANUBICE: // Anubis
                DomRod_ControlAnubis(p, play, domRodControlledActor);
                break;
            default:
                DomRod_EndControl(p, play);
                return;
        }
    }

    // Clear first frame flag
    sControlFirstFrame = 0;

    DomRod_UpdateLight();

    if ((play->gameplayFrames % 5) == 0) {
        DomRod_SpawnOrbParticles(play);
    }
}

static void DomRod_StateCatching(Player* p, PlayState* play) {
    if (DECR(domRodTimer) == 0) {
        DomRod_RemoveLight(play);
        domRodState = DOMROD_STATE_IDLE;
        domRodActive = 0;
    }
}

// ============================================================================
// MAIN HANDLER
// ============================================================================

void Handle_DominionRod(Player* p, PlayState* play) {
    if (!sDomRodColInitialized) DomRod_InitCollider(play, p);

    ItemInputState in;
    ItemInput_Update(&in, ITEM_DOMINION_ROD, p, play);
    domRodButtonMask = in.equippedButton;

    if (!in.wasEquipped) {
        if (domRodActive) DomRod_Stop(p, play);
        return;
    }

    if (domRodState != DOMROD_STATE_CONTROLLING &&
        domRodState != DOMROD_STATE_ORB_FLYING &&
        domRodState != DOMROD_STATE_ORB_RETURN &&
        domRodState != DOMROD_STATE_CATCHING) {
        if (ItemInput_IsBlocked(p, play)) {
            if (domRodActive) DomRod_Stop(p, play);
            return;
        }
    }

    if (domRodState == DOMROD_STATE_CONTROLLING) {
        // Control mode: pause on damage but don't stop
    } else {
        if (ItemInput_CheckDamage(p, &domRodPrevInvinc)) {
            if (domRodState == DOMROD_STATE_ORB_FLYING) {
                DomRod_StartReturn(p, play);
            } else if (domRodActive && domRodState != DOMROD_STATE_ORB_RETURN &&
                       domRodState != DOMROD_STATE_CATCHING) {
                DomRod_Stop(p, play);
            }
            return;
        }
    }

    if (!domRodActive) {
        if (in.isPressed) DomRod_Start(p, play);
        return;
    }

    if (domRodState == DOMROD_STATE_AIMING &&
        CHECK_BTN_ALL(play->state.input[0].press.button, BTN_B)) {
        DomRod_Stop(p, play);
        return;
    }

    switch (domRodState) {
        case DOMROD_STATE_AIMING:
            DomRod_StateAiming(p, play, &in);
            break;
        case DOMROD_STATE_ORB_FLYING:
            DomRod_StateOrbFlying(p, play);
            break;
        case DOMROD_STATE_ORB_RETURN:
            DomRod_StateOrbReturning(p, play);
            break;
        case DOMROD_STATE_CONTROLLING:
            DomRod_StateControlling(p, play, &in);
            break;
        case DOMROD_STATE_CATCHING:
            DomRod_StateCatching(p, play);
            break;
        default:
            domRodState = DOMROD_STATE_IDLE;
            domRodActive = 0;
            break;
    }
}

// ============================================================================
// INIT & UPPER ACTION
// ============================================================================

void Player_InitDominionRodIA(PlayState* play, Player* p) {
    DomRod_InitCollider(play, p);
    domRodActive = 0;
    domRodState = DOMROD_STATE_IDLE;
    domRodFirstPerson = 0;
    domRodControlledActor = NULL;
    domRodTimer = 0;
    domRodLightNode = NULL;
    domRodControlType = DOMROD_CONTROL_NONE;
    domRodDamagePaused = 0;
    domRodAttackCooldown = 0;
    domRodCButtonHoldTimer = 0;
    sLastLinkPos.x = 0;
    sLastLinkPos.y = 0;
    sLastLinkPos.z = 0;
    sLinkWasJumping = 0;
}

s32 Player_UpperAction_DominionRod(Player* this, PlayState* play) {
    return 0;
}

// ============================================================================
// DRAW FUNCTION
// ============================================================================

void CustomItems_DrawDominionRod(Player* p, PlayState* play) {
    if (!domRodActive) return;
    if (domRodState != DOMROD_STATE_ORB_FLYING &&
        domRodState != DOMROD_STATE_ORB_RETURN &&
        domRodState != DOMROD_STATE_CONTROLLING) return;

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Translate(domRodOrbPos.x, domRodOrbPos.y, domRodOrbPos.z, MTXMODE_NEW);
    Matrix_ReplaceRotation(&play->billboardMtxF);
    Matrix_Scale(DOMROD_ORB_SCALE, DOMROD_ORB_SCALE, DOMROD_ORB_SCALE, MTXMODE_APPLY);

    s16 rotZ = (play->gameplayFrames * 0x1000) + (s16)(Rand_ZeroOne() * 0x4000);
    Matrix_RotateZ((rotZ / (f32)0x8000) * M_PI, MTXMODE_APPLY);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0,
        DOMROD_ORB_PRIM_R, DOMROD_ORB_PRIM_G, DOMROD_ORB_PRIM_B, DOMROD_ORB_PRIM_A);
    gDPSetEnvColor(POLY_XLU_DISP++,
        DOMROD_ORB_ENV_R, DOMROD_ORB_ENV_G, DOMROD_ORB_ENV_B, DOMROD_ORB_ENV_A);
    gDPPipeSync(POLY_XLU_DISP++);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
        G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gPhantomEnergyBallDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

// ============================================================================
// RETICLE DRAW FUNCTION
// ============================================================================

void CustomItems_DrawDominionRodReticle(Player* p, PlayState* play) {
    if (!domRodActive || !domRodFirstPerson) return;
    if (domRodState != DOMROD_STATE_AIMING) return;

    FirstPerson_DrawReticle(p, play, 0.0f, DOMROD_RETICLE_R, DOMROD_RETICLE_G, DOMROD_RETICLE_B);
}

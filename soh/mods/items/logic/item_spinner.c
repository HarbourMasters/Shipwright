/**
 * item_spinner.c - Spinner from Twilight Princess
 *
 * Controls:
 *   C Button: Toggle riding on/off
 *   A Button (riding): Homing dash attack toward nearest enemy
 *   Analog:   Steer direction while riding
 *
 * Features:
 *   - Rideable vehicle with constant spinning animation
 *   - Homing attack damages enemies and breaks rocks
 *   - Speed boost when charging toward targets
 *   - Cucco easter egg interaction
 */

#include "z64.h"
#include "item_spinner.h"
#include "../custom_items.h"
#include "../helpers/equip_helper.h"
#include "../helpers/combat_helper.h"
#include "../helpers/fx_helper.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "objects/gameplay_keep/gameplay_keep.h"

static ColliderCylinder sSpinnerCol;
static u8 sSpinnerColInitialized = 0;
static s8 sSpinnerPrevInvinc = 0;

static void Spinner_InitCollider(PlayState* play, Player* p) {
    if (sSpinnerColInitialized) return;
    CombatColliderConfig cfg = { DMG_SLASH_MASTER, SPINNER_DMG_RIDE, 0, SPINNER_COL_RADIUS, SPINNER_COL_HEIGHT };
    Combat_InitCylinder(play, &sSpinnerCol, &p->actor, &cfg);
    sSpinnerColInitialized = 1;
}

static void Spinner_UpdateCollider(Player* p, PlayState* play, s16 r, u8 dmg) {
    Vec3f pos = { p->actor.world.pos.x, p->actor.world.pos.y - 25 + SPINNER_HOVER_HEIGHT, p->actor.world.pos.z };
    CombatColliderConfig cfg = { DMG_SLASH_MASTER, dmg, 0, r, SPINNER_COL_HEIGHT };
    Combat_UpdateCylinder(&sSpinnerCol, &pos, &cfg);
    Combat_RegisterCollider(play, &sSpinnerCol);
    CollisionCheck_SetOC(play, &play->colChkCtx, &sSpinnerCol.base);
}

static void Spinner_CheckHit(Player* p) {
    if (Combat_CheckHit(&sSpinnerCol)) {
        Combat_PlayHitSFX(&p->actor.world.pos);
        sSpinnerCol.base.atFlags &= ~AT_HIT;
    }
}

static void BreakRock(PlayState* play, Actor* rock) {
    if (!rock || !rock->update) return;
    Vec3f pos = rock->world.pos;

    // Boulder destruction VFX (dust clouds, same as Obj_Hamishi)
    func_80033480(play, &pos, 140.0f, 6, 180, 90, 1);
    func_80033480(play, &pos, 140.0f, 12, 80, 90, 1);

    // Boulder destruction SFX
    SoundSource_PlaySfxAtFixedWorldPos(play, &pos, 40, NA_SE_EV_WALL_BROKEN);

    Actor_Kill(rock);
}

static u8 IsBreakableRock(s16 id) {
    for (u32 i = 0; i < BREAKABLE_ROCK_COUNT; i++)
        if (sBreakableRockIds[i] == id) return 1;
    return 0;
}

static void CheckRocks(Player* p, PlayState* play, f32 r) {
    f32 rSq = SQ(r);
    for (int cat = 0; cat < 2; cat++) {
        Actor* a = play->actorCtx.actorLists[cat == 0 ? ACTORCAT_PROP : ACTORCAT_BG].head;
        while (a) {
            Actor* next = a->next;
            if (a->update && IsBreakableRock(a->id)) {
                f32 dx = a->world.pos.x - p->actor.world.pos.x;
                f32 dz = a->world.pos.z - p->actor.world.pos.z;
                if (SQ(dx) + SQ(dz) < rSq) BreakRock(play, a);
            }
            a = next;
        }
    }
}

static void CuccoEasterEgg(Player* p, PlayState* play, Actor* cucco) {
    if (!cucco || !cucco->update) return;
    sActive = 0; sState = SPINNER_STATE_IDLE;
    s16 angle = Math_Vec3f_Yaw(&cucco->world.pos, &p->actor.world.pos);
    p->actor.world.pos.x += Math_SinS(angle) * 50.0f;
    p->actor.world.pos.z += Math_CosS(angle) * 50.0f;
    p->actor.velocity.y = 8.0f; p->actor.gravity = -1.2f;
    p->stateFlags1 &= ~PLAYER_STATE1_CHARGING_SPIN_ATTACK;
    p->stateFlags2 &= ~PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;
    Audio_PlayActorSound2(&p->actor, NA_SE_PL_DAMAGE);
    Audio_PlayActorSound2(cucco, NA_SE_EV_CHICKEN_CRY_M);
}

static void CheckCucco(Player* p, PlayState* play) {
    f32 rSq = SQ(SPINNER_COL_RADIUS_ATK + 30);
    for (int cat = 0; cat < 2; cat++) {
        Actor* a = play->actorCtx.actorLists[cat == 0 ? ACTORCAT_PROP : ACTORCAT_ENEMY].head;
        while (a) {
            if (a->update && (a->id == ACTOR_EN_NIW || a->id == ACTOR_EN_ATTACK_NIW)) {
                f32 dx = a->world.pos.x - p->actor.world.pos.x;
                f32 dz = a->world.pos.z - p->actor.world.pos.z;
                if (SQ(dx) + SQ(dz) < rSq) { CuccoEasterEgg(p, play, a); return; }
            }
            a = a->next;
        }
    }
}

static void Spinner_Stop(Player* p, PlayState* play) {
    sActive = 0; sState = SPINNER_STATE_IDLE;
    p->actor.gravity = -1.2f;
    p->actor.shape.rot.x = 0;
    p->actor.shape.rot.z = 0;
    p->stateFlags1 &= ~PLAYER_STATE1_CHARGING_SPIN_ATTACK;
    p->stateFlags2 &= ~PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;
    sSpinnerCol.base.atFlags &= ~AT_ON;
    Audio_PlaySoundGeneral(NA_SE_PL_LAND, &p->actor.world.pos, 4,
        &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

static void Spinner_Start(Player* p, PlayState* play) {
    sActive = 1; sCharge = 0; sState = SPINNER_STATE_CHARGING;
    p->actor.velocity.y = 8.0f;
    ItemInput_RequestItemChange(p, play);
    Audio_PlaySoundGeneral(NA_SE_IT_SWORD_SWING, &p->actor.world.pos, 4,
        &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

static void SetFlags(Player* p) {
    p->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;
    p->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;
}

static void Hover(Player* p, f32 extraH) {
    f32 targetY = p->actor.floorHeight + SPINNER_HOVER_HEIGHT + SPINNER_Y_OFFSET + extraH;
    Math_StepToF(&p->actor.world.pos.y, targetY, 4.0f);
    p->actor.velocity.y = 0.0f; p->actor.gravity = 0.0f;
}

static void SetAnimation(Player* p, PlayState* play, f32 speed) {
    if (p->skelAnime.animation != (LinkAnimationHeader*)&gPlayerAnim_link_fighter_Lpower_kiru_wait) {
        LinkAnimation_Change(play, &p->skelAnime, (LinkAnimationHeader*)&gPlayerAnim_link_fighter_Lpower_kiru_wait,
            speed, 0.0f, Animation_GetLastFrame((LinkAnimationHeader*)&gPlayerAnim_link_fighter_Lpower_kiru_wait),
            ANIMMODE_LOOP, -4.0f);
    }
}

static void Spinner_StateCharging(Player* p, PlayState* play, ItemInputState* in) {
    SetFlags(p);
    if (in->isHeld && sCharge < SPINNER_CHARGE_MAX) sCharge++;
    p->actor.speedXZ = 0.0f; p->linearVelocity = 0.0f;
    Hover(p, 0.0f);
    SetAnimation(p, play, 1.0f);

    // Charge particles disabled for now

    if ((play->gameplayFrames % 20) == 0)
        Audio_PlaySoundGeneral(NA_SE_IT_SHIELD_BOUND, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

    if (!in->isHeld) {
        f32 ratio = (f32)sCharge / SPINNER_CHARGE_MAX;
        sSpeed = SPINNER_SPEED_MIN + (SPINNER_SPEED_MAX - SPINNER_SPEED_MIN) * ratio;

        if (p->focusActor && (p->stateFlags1 & PLAYER_STATE1_Z_TARGETING)) {
            sAngle = Math_Vec3f_Yaw(&p->actor.world.pos, &p->focusActor->world.pos);
            sTarget = p->focusActor; sHomingTime = 0;
            sState = SPINNER_STATE_HOMING_WINDUP;
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_PUTAWAY, &p->actor.world.pos, 4,
                &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        } else {
            sAngle = p->actor.shape.rot.y; sTarget = NULL;
            p->actor.world.rot.y = p->actor.shape.rot.y = sAngle;
            sTimer = SPINNER_RIDE_DURATION; sAtkTimer = 0;
            sState = SPINNER_STATE_RIDING;
            Audio_PlaySoundGeneral(NA_SE_IT_HAMMER_SWING, &p->actor.world.pos, 4,
                &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        }
        p->actor.velocity.y = 6.0f;
    }
}

static void StateHomingWindup(Player* p, PlayState* play) {
    SetFlags(p); sHomingTime++;
    p->actor.speedXZ = 0.0f; p->linearVelocity = 0.0f;
    Hover(p, 0.0f);

    if ((play->gameplayFrames % 8) == 0)
        Audio_PlaySoundGeneral(NA_SE_IT_SHIELD_BOUND, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

    if (sHomingTime >= HOMING_WINDUP_DURATION) {
        sState = SPINNER_STATE_HOMING_AIM; sHomingTime = 0;
        Audio_PlaySoundGeneral(NA_SE_IT_SWORD_SWING_HARD, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }
}

static void StateHomingAim(Player* p, PlayState* play) {
    SetFlags(p); sHomingTime++;
    p->actor.speedXZ = 0.0f; p->linearVelocity = 0.0f;
    Hover(p, 0.0f);

    // Lock on target angle (no Link rotation)
    if (sTarget && sTarget->update) {
        sAngle = Math_Vec3f_Yaw(&p->actor.world.pos, &sTarget->world.pos);
    }

    if (sHomingTime >= HOMING_AIM_DURATION) {
        sState = SPINNER_STATE_HOMING_LAUNCH; sHomingTime = 0;
        sSpeed = SPINNER_SPEED_HOMING;
        Audio_PlaySoundGeneral(NA_SE_IT_HAMMER_SWING, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }
}

static void StateHomingLaunch(Player* p, PlayState* play) {
    SetFlags(p); sHomingTime++;

    // Arc movement
    f32 prog = sHomingTime / 30.0f; if (prog > 1.0f) prog = 1.0f;
    f32 arc = Math_SinS((s16)(prog * 0x8000)) * SPINNER_HOMING_ARC;
    Hover(p, arc);
    SetAnimation(p, play, 3.0f);

    // Move toward target
    if (sTarget && sTarget->update) {
        sAngle = Math_Vec3f_Yaw(&p->actor.world.pos, &sTarget->world.pos);
    }

    p->actor.world.pos.x += Math_SinS(sAngle) * sSpeed;
    p->actor.world.pos.z += Math_CosS(sAngle) * sSpeed;

    Spinner_UpdateCollider(p, play, SPINNER_COL_RADIUS_HOME, SPINNER_DMG_HOMING);
    Spinner_CheckHit(p); CheckRocks(p, play, SPINNER_COL_RADIUS_HOME + 40); CheckCucco(p, play);

    // Check if hit target or timeout
    u8 hitTarget = 0;
    if (sTarget && sTarget->update) {
        f32 dist = Math_Vec3f_DistXYZ(&p->actor.world.pos, &sTarget->world.pos);
        if (dist < SPINNER_COL_RADIUS_HOME) hitTarget = 1;
    }

    if (hitTarget || sHomingTime > 30) {
        if (hitTarget) {
            // Shockwave at target's floor level
            Vec3f impactPos;
            impactPos.x = sTarget->world.pos.x;
            impactPos.y = sTarget->floorHeight + 2.0f;
            impactPos.z = sTarget->world.pos.z;
            FX_SpawnShockwaveSmall(play, &impactPos, 60, 150);
            Audio_PlaySoundGeneral(NA_SE_IT_HAMMER_HIT, &impactPos, 4,
                &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        }
        sTarget = NULL; sTimer = SPINNER_RECOIL_DURATION; sState = SPINNER_STATE_RECOIL;
        return;
    }

    // Trail particles disabled for now

    if ((play->gameplayFrames % 6) == 0)
        Audio_PlaySoundGeneral(NA_SE_EV_ROCK_SLIDE, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

static void StateRiding(Player* p, PlayState* play, ItemInputState* in) {
    SetFlags(p);
    if (sTimer > 0) sTimer--;
    SetAnimation(p, play, 1.0f);

    // Press item button again to attack
    if (in->isPressed && sAtkTimer == 0) {
        sState = SPINNER_STATE_ATTACKING; sAtkTimer = SPINNER_ATTACK_DURATION;
        Audio_PlayActorSound2(&p->actor, NA_SE_VO_LI_MAGIC_ATTACK);
        Audio_PlaySoundGeneral(NA_SE_IT_HAMMER_SWING, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        // Shockwave at Link's feet (floorHeight)
        Vec3f pos;
        pos.x = p->actor.world.pos.x;
        pos.y = p->actor.floorHeight + 2.0f;
        pos.z = p->actor.world.pos.z;
        FX_SpawnShockwaveSmall(play, &pos, 60, 150);
        return;
    }

    f32 stickX = play->state.input[0].rel.stick_x;
    if (fabsf(stickX) > 10.0f) sAngle += (s16)(stickX * 6.0f);

    if (p->actor.bgCheckFlags & 0x0008) {
        sAngle += 0x8000;
        Audio_PlaySoundGeneral(NA_SE_IT_SHIELD_BOUND, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }

    p->actor.world.pos.x += Math_SinS(sAngle) * sSpeed;
    p->actor.world.pos.z += Math_CosS(sAngle) * sSpeed;
    p->actor.world.rot.y = p->actor.shape.rot.y = sAngle;
    Hover(p, 0.0f);

    // Riding does no damage and doesn't break rocks

    if ((play->gameplayFrames % 20) == 0)
        Audio_PlaySoundGeneral(NA_SE_EV_ROCK_SLIDE, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

    if (sTimer <= 0) Spinner_Stop(p, play);
}

static void StateAttacking(Player* p, PlayState* play) {
    SetFlags(p);
    if (sAtkTimer > 0) sAtkTimer--; else { sState = SPINNER_STATE_RIDING; return; }
    SetAnimation(p, play, 3.0f);

    f32 stickX = play->state.input[0].rel.stick_x;
    if (fabsf(stickX) > 15.0f) sAngle += (s16)(stickX * 3.0f);

    if (p->actor.bgCheckFlags & 0x0008) {
        sAngle += 0x8000;
        Audio_PlaySoundGeneral(NA_SE_IT_SHIELD_BOUND, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }

    p->actor.world.pos.x += Math_SinS(sAngle) * sSpeed * 1.3f;
    p->actor.world.pos.z += Math_CosS(sAngle) * sSpeed * 1.3f;
    p->actor.world.rot.y = p->actor.shape.rot.y = sAngle;
    Hover(p, 10.0f);

    Spinner_UpdateCollider(p, play, SPINNER_COL_RADIUS_ATK, SPINNER_DMG_ATTACK);
    Spinner_CheckHit(p); CheckRocks(p, play, SPINNER_COL_RADIUS_ATK + 40); CheckCucco(p, play);

    if ((play->gameplayFrames % 8) == 0)
        Audio_PlaySoundGeneral(NA_SE_IT_SWORD_SWING, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

static void StateRecoil(Player* p, PlayState* play) {
    SetFlags(p);
    if (sTimer > 0) sTimer--; else { Spinner_Stop(p, play); return; }
    SetAnimation(p, play, 0.5f);

    sSpeed *= 0.9f;
    p->actor.world.pos.x += Math_SinS(sAngle + 0x8000) * sSpeed * 0.3f;
    p->actor.world.pos.z += Math_CosS(sAngle + 0x8000) * sSpeed * 0.3f;
    Hover(p, 0.0f);
}

void Handle_Spinner(Player* p, PlayState* play) {
    if (!sSpinnerColInitialized) Spinner_InitCollider(play, p);

    ItemInputState in;
    ItemInput_Update(&in, ITEM_SPINNER, p, play);

    if (!in.wasEquipped) { if (sActive) Spinner_Stop(p, play); return; }
    if (ItemInput_IsBlockedEx(p, play, 1)) { if (sActive) Spinner_Stop(p, play); return; }
    if (ItemInput_CheckDamage(p, &sSpinnerPrevInvinc)) { if (sActive) Spinner_Stop(p, play); return; }

    // Not active - check if we should start
    if (!sActive) {
        if (p->stateFlags1 & PLAYER_STATE1_IN_WATER) return;
        if (p->meleeWeaponState != 0) return;
        if (in.isPressed && (p->actor.bgCheckFlags & 0x0001)) Spinner_Start(p, play);
        return;
    }

    // State machine
    switch (sState) {
        case SPINNER_STATE_CHARGING:
            Spinner_StateCharging(p, play, &in);
            break;
        case SPINNER_STATE_RIDING:
            StateRiding(p, play, &in);
            break;
        case SPINNER_STATE_ATTACKING:
            StateAttacking(p, play);
            break;
        case SPINNER_STATE_HOMING_WINDUP:
            StateHomingWindup(p, play);
            break;
        case SPINNER_STATE_HOMING_AIM:
            StateHomingAim(p, play);
            break;
        case SPINNER_STATE_HOMING_LAUNCH:
            StateHomingLaunch(p, play);
            break;
        case SPINNER_STATE_RECOIL:
            StateRecoil(p, play);
            break;
        default:
            Spinner_Stop(p, play);
            break;
    }
}

void Player_InitSpinnerIA(PlayState* play, Player* p) {
    // Only initialize if collider not already set up (prevents resetting active state)
    if (!sSpinnerColInitialized) {
        Spinner_InitCollider(play, p);
        sActive = 0; sCharge = 0; sState = SPINNER_STATE_IDLE;
        sAtkTimer = 0; sAngle = 0; sTimer = 0; sSpeed = 0.0f;
        sTarget = NULL; sHomingTime = 0;
    }
}

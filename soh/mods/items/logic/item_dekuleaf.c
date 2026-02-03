/**
 * item_dekuleaf.c - Deku Leaf from Wind Waker
 *
 * Controls:
 *   C Button (ground): Swing leaf to create wind gust, pushes objects/enemies
 *   C Button (air):    Hold to glide, consumes magic over time
 *
 * Features:
 *   - Wind blow pushes enemies and certain objects
 *   - Gliding reduces fall speed and allows horizontal movement
 *   - Uses skeletal animation (39 frames) for blow attack
 */

#include "z64.h"
#include "item_dekuleaf.h"
#include "../custom_items.h"
#include "../helpers/movement_helper.h"
#include "../helpers/equip_helper.h"
#include "../helpers/fx_helper.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "objects/gameplay_keep/gameplay_keep.h"

// Include animation
#include "../anim/deku_leaf/dekuleaf_anim.c"
#include "../anim/deku_leaf/dekuleaf_anim_data.c"

static s8 sDekuLeafPrevInvinc = 0;
static u8 sDekuLeafBlowEffectFired = 0;

static void DekuLeaf_Stop(Player* p, PlayState* play) {
    if (!dlActive) return;

    dlActive = 0;
    dlMode = DEKULEAF_MODE_INACTIVE;
    dlGliding = 0;
    dlBlowing = 0;
    dlAnimTimer = 0;
    dlBlowTimer = 0;
    sDekuLeafBlowEffectFired = 0;

    p->stateFlags1 &= ~PLAYER_STATE1_INPUT_DISABLED;
    ItemEquip_PlayUnequipSFX(play, p);
}

static void DekuLeaf_StartGlide(Player* p, PlayState* play) {
    dlActive = 1;
    dlMode = DEKULEAF_MODE_GLIDING;
    dlGliding = 1;
    dlBlowing = 0;
    dlAnimTimer = 0;
    ItemEquip_PlayEquipSFX(play, p);
}

static void DekuLeaf_StartBlow(Player* p, PlayState* play) {
    if (gSaveContext.magic < DEKULEAF_BLOW_MAGIC_COST) return;

    dlActive = 1;
    dlMode = DEKULEAF_MODE_BLOWING;
    dlGliding = 0;
    dlBlowing = 1;
    dlAnimTimer = 0;
    dlBlowTimer = 0;
    sDekuLeafBlowEffectFired = 0;

    // Start the skeletal animation on upperSkelAnime
    LinkAnimation_PlayOnce(play, &p->upperSkelAnime, &gDekuLeafBlowAnim);

    ItemEquip_PlayEquipSFX(play, p);
}

static void DekuLeaf_UpdateGlide(Player* p, PlayState* play) {
    if (p->skelAnime.animation != &DEKULEAF_ANIM_GLIDE) {
        LinkAnimation_Change(play, &p->skelAnime, &DEKULEAF_ANIM_GLIDE, 1.0f, 0.0f,
            Animation_GetLastFrame(&DEKULEAF_ANIM_GLIDE), ANIMMODE_LOOP, -4.0f);
    }

    if (p->actor.velocity.y < DEKULEAF_FALL_VELOCITY) {
        p->actor.velocity.y = DEKULEAF_FALL_VELOCITY;
    }

    if (play->gameplayFrames % DEKULEAF_GLIDE_MAGIC_INTERVAL == 0) {
        ItemMagic_Consume(play, DEKULEAF_GLIDE_MAGIC_COST);
    }

    if (play->gameplayFrames % 10 == 0) {
        Player_PlaySfx(p, DEKULEAF_SOUND_WIND);
    }
}

static void DekuLeaf_SpawnWindParticles(Player* p, PlayState* play) {
    Vec3f windPos = p->actor.world.pos;
    s16 facingYaw = p->actor.shape.rot.y;

    windPos.y += 30.0f;

    FX_SpawnWindBlow(play, &windPos, facingYaw, DEKULEAF_BLOW_RANGE);
}

static void DekuLeaf_BlowEnemies(Player* p, PlayState* play) {
    Vec3f blowOrigin = p->actor.world.pos;
    s16 facingYaw = p->actor.shape.rot.y;

    blowOrigin.y += 30.0f;

    Actor* actor = play->actorCtx.actorLists[ACTORCAT_ENEMY].head;
    while (actor != NULL) {
        if (actor->update != NULL) {
            f32 dx = actor->world.pos.x - blowOrigin.x;
            f32 dz = actor->world.pos.z - blowOrigin.z;
            f32 dist = sqrtf(SQ(dx) + SQ(dz));
            f32 dy = fabsf(actor->world.pos.y - blowOrigin.y);

            if (dist < DEKULEAF_BLOW_RANGE && dy < 60.0f) {
                s16 angleToEnemy = Math_Atan2S(dx, dz);
                s16 angleDiff = angleToEnemy - facingYaw;

                if (angleDiff > -0x4000 && angleDiff < 0x4000) {
                    f32 forceMult = 1.0f - (dist / DEKULEAF_BLOW_RANGE);
                    f32 force = DEKULEAF_BLOW_FORCE * forceMult;

                    f32 norm = dist > 0.1f ? dist : 0.1f;
                    actor->world.pos.x += (dx / norm) * force;
                    actor->world.pos.z += (dz / norm) * force;

                    if (actor->bgCheckFlags & BGCHECKFLAG_GROUND) {
                        actor->world.pos.y += 3.0f * forceMult;
                    }
                }
            }
        }
        actor = actor->next;
    }
}

// ============================================================================
// UPPER ACTION - Drives the blow animation via upperSkelAnime
// ============================================================================

s32 Player_UpperAction_DekuLeaf(Player* player, PlayState* play) {
    if (!dlActive) return 0;
    if (!dlBlowing) return 0;

    // Update the skeletal animation
    if (LinkAnimation_Update(play, &player->upperSkelAnime)) {
        // Animation finished
        DekuLeaf_Stop(player, play);
        return 0;
    }

    // Track frame
    dlAnimTimer++;

    // Stop movement during blow animation
    player->stateFlags1 |= PLAYER_STATE1_INPUT_DISABLED;
    player->actor.speedXZ = 0.0f;
    player->linearVelocity = 0.0f;

    // Fire the wind blow effect at the designated frame
    if (!sDekuLeafBlowEffectFired && dlAnimTimer == DEKULEAF_BLOW_EFFECT_FRAME) {
        sDekuLeafBlowEffectFired = 1;
        ItemMagic_Consume(play, DEKULEAF_BLOW_MAGIC_COST);
        Player_PlaySfx(player, DEKULEAF_SOUND_BLOW);
        dlBlowTimer = DEKULEAF_BLOW_DURATION;
    }

    // During active blow frames, spawn wind and push enemies
    if (dlAnimTimer >= DEKULEAF_ATTACK_FRAME_START && dlAnimTimer <= DEKULEAF_ATTACK_FRAME_END) {
        if (play->gameplayFrames % DEKULEAF_WIND_SPAWN_RATE == 0) {
            DekuLeaf_SpawnWindParticles(player, play);
        }
        DekuLeaf_BlowEnemies(player, play);
    }

    // Return 1 to indicate upper body is busy (use upperSkelAnime)
    return 1;
}

// ============================================================================
// MAIN HANDLER
// ============================================================================

void Handle_DekuLeaf(Player* p, PlayState* play) {
    ItemInputState in;
    ItemInput_Update(&in, ITEM_DEKU_LEAF, p, play);

    if (!in.wasEquipped) {
        if (dlActive) DekuLeaf_Stop(p, play);
        return;
    }

    if (ItemInput_IsBlocked(p, play)) {
        if (dlActive) DekuLeaf_Stop(p, play);
        return;
    }

    if (ItemInput_CheckDamage(p, &sDekuLeafPrevInvinc)) {
        DekuLeaf_Stop(p, play);
        return;
    }

    // If blowing, the upper action handles everything
    if (dlMode == DEKULEAF_MODE_BLOWING) {
        return;
    }

    if (dlMode == DEKULEAF_MODE_GLIDING) {
        if (Movement_IsOnGround(p)) {
            DekuLeaf_Stop(p, play);
            return;
        }

        if (!in.isHeld || gSaveContext.magic <= 0 || in.otherButtonPressed) {
            DekuLeaf_Stop(p, play);
            return;
        }

        DekuLeaf_UpdateGlide(p, play);
        return;
    }

    if (!dlActive && in.isPressed) {
        if (!Movement_IsOnGround(p)) {
            if (gSaveContext.magic > 0) {
                DekuLeaf_StartGlide(p, play);
            }
        } else {
            if (p->stateFlags1 & PLAYER_STATE1_IN_WATER) return;
            if (p->meleeWeaponState != 0) return;
            DekuLeaf_StartBlow(p, play);
        }
        return;
    }

    if (dlMode == DEKULEAF_MODE_GLIDING && !in.isHeld) {
        DekuLeaf_Stop(p, play);
    }
}

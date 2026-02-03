/**
 * item_zonai_permafrost.c - Zonai Permafrost (time freeze)
 *
 * Controls:
 *   C Button: Cast time freeze (12 MP cost)
 *
 * Features:
 *   - Freezes all actors and day/night cycle for 10 seconds
 *   - Link moves freely during effect
 *   - Din's Fire style cast animation
 *   - Green Zonai energy runes visual
 */

#include "z64.h"
#include "item_zonai_permafrost.h"
#include "../custom_items.h"
#include "../helpers/equip_helper.h"
#include "../helpers/fx_helper.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "objects/gameplay_keep/gameplay_keep.h"

// Vanilla function: resets player actionFunc to idle (or lock-on variant)
extern void func_80839FFC(Player* this, PlayState* play);

static s8 sZPermPrevInvinc = 0;
static s32 sZPermPhaseEnd = 0;

// Actor categories to freeze
static const u8 sFreezeCats[] = {
    ACTORCAT_SWITCH,      // 0x00 - buttons, switches
    ACTORCAT_BG,          // 0x01 - background actors
    ACTORCAT_EXPLOSIVE,   // 0x03 - bombs
    ACTORCAT_NPC,         // 0x04 - NPCs
    ACTORCAT_ENEMY,       // 0x05 - enemies
    ACTORCAT_ITEMACTION,  // 0x07 - projectiles
    ACTORCAT_MISC,        // 0x08 - misc
    ACTORCAT_BOSS,        // 0x09 - bosses
};

// ============================================================================
// Freeze / Unfreeze Helpers
// ============================================================================

/**
 * Set freezeTimer on all actors in target categories.
 * Called EVERY FRAME during ACTIVE to keep actors frozen.
 * DECR() decrements freezeTimer each frame, so we re-set it to
 * ZPERM_FREEZE_REFRESH (3) to stay ahead of the decrement.
 * This also catches newly spawned actors.
 */
static void ZPerm_FreezeAllTargets(PlayState* play) {
    ActorContext* actorCtx = &play->actorCtx;
    for (u32 i = 0; i < ARRAY_COUNT(sFreezeCats); i++) {
        Actor* actor = actorCtx->actorLists[sFreezeCats[i]].head;
        while (actor != NULL) {
            actor->freezeTimer = ZPERM_FREEZE_REFRESH;
            actor = actor->next;
        }
    }
}

/**
 * Clear freezeTimer on all actors in target categories.
 * Called once when effect ends so actors resume immediately.
 */
static void ZPerm_UnfreezeAllTargets(PlayState* play) {
    ActorContext* actorCtx = &play->actorCtx;
    for (u32 i = 0; i < ARRAY_COUNT(sFreezeCats); i++) {
        Actor* actor = actorCtx->actorLists[sFreezeCats[i]].head;
        while (actor != NULL) {
            actor->freezeTimer = 0;
            actor = actor->next;
        }
    }
}

// ============================================================================
// Visual Effects
// ============================================================================

/**
 * Green rune particles expanding outward during casting.
 * 8 sparkles in a ring pattern at the given radius.
 */
static void ZPerm_SpawnCastRunes(Player* p, PlayState* play, f32 expandRadius) {
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Color_RGBA8 primColor = { 100, 255, 150, 255 };  // Bright Zonai green
    Color_RGBA8 envColor  = { 0, 200, 80, 255 };     // Deep green

    for (u8 i = 0; i < 8; i++) {
        f32 angle = (f32)i * (65536.0f / 8.0f);
        s16 angleS = (s16)angle;

        Vec3f pos;
        pos.x = p->actor.world.pos.x + Math_SinS(angleS) * expandRadius;
        pos.y = p->actor.world.pos.y + 30.0f + Rand_CenteredFloat(20.0f);
        pos.z = p->actor.world.pos.z + Math_CosS(angleS) * expandRadius;

        Vec3f vel;
        vel.x = Math_SinS(angleS) * 3.0f;
        vel.y = Rand_ZeroFloat(1.5f);
        vel.z = Math_CosS(angleS) * 3.0f;

        EffectSsKiraKira_SpawnFocused(play, &pos, &vel, &accel, &primColor, &envColor, 600, 20);
    }
}

/**
 * Green particles floating frozen in air during active phase.
 * 2 particles per frame at random positions around Link.
 */
static void ZPerm_SpawnFrozenParticles(Player* p, PlayState* play) {
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f vel   = { 0.0f, 0.0f, 0.0f };
    Color_RGBA8 primColor = { 120, 255, 160, 200 };
    Color_RGBA8 envColor  = { 0, 180, 60, 150 };

    for (u8 i = 0; i < 2; i++) {
        Vec3f pos;
        pos.x = p->actor.world.pos.x + Rand_CenteredFloat(300.0f);
        pos.y = p->actor.world.pos.y + 20.0f + Rand_ZeroFloat(100.0f);
        pos.z = p->actor.world.pos.z + Rand_CenteredFloat(300.0f);

        EffectSsKiraKira_SpawnFocused(play, &pos, &vel, &accel, &primColor, &envColor, 400, 15);
    }
}

// ============================================================================
// Stop / Start
// ============================================================================

static void ZPerm_Stop(Player* p, PlayState* play) {
    if (!zpActive) return;

    // If in ACTIVE or ENDING, unfreeze actors and restore time
    if (zpState == ZPERM_STATE_ACTIVE || zpState == ZPERM_STATE_ENDING) {
        ZPerm_UnfreezeAllTargets(play);
        gTimeIncrement = zpSavedTime;
    }

    // If in CASTING, release player lock, reset action, and restore camera
    if (zpState == ZPERM_STATE_CASTING) {
        p->stateFlags1 &= ~(PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED);
        func_80839FFC(p, play);
        func_8005B1A4(Play_GetCamera(play, 0));
    }

    zpActive = 0;
    zpState = ZPERM_STATE_IDLE;
    zpSubPhase = 0;
    zpTimer = 0;
    zpSavedTime = 0;
}

static void ZPerm_Start(Player* p, PlayState* play) {
    if (zpActive) return;

    if (!ItemMagic_HasEnough(play, ZPERM_MAGIC_COST)) {
        Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        return;
    }

    if (!(p->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) return;

    zpActive = 1;
    zpState = ZPERM_STATE_CASTING;
    zpSubPhase = ZPERM_CAST_HONOO1;
    zpTimer = -2;
    ItemMagic_Consume(play, ZPERM_MAGIC_COST);
}

// ============================================================================
// State: Casting (Din's Fire honoo1 -> honoo2 -> honoo3)
//
// Uses DEMISE PATTERN for reliable animation handling:
// 1. LinkAnimation_Change at 0.415f (half vanilla speed)
// 2. Explicit LinkAnimation_Update call (double-update with vanilla's call)
//    → effective speed ≈ vanilla's 0.83
// 3. Timer-based chaining computed from R_UPDATE_RATE (never relies on animDone)
// ============================================================================

static void ZPerm_ComputePhaseEnd(s32 baseTimer, f32 lastFrame) {
    f32 rate = ZPERM_ANIM_SPEED * R_UPDATE_RATE;
    if (rate < 0.1f) rate = 0.415f;  // Safety fallback
    sZPermPhaseEnd = baseTimer + (s32)(lastFrame / rate) + 1;
}

static void ZPerm_StateCasting(Player* p, PlayState* play) {
    zpTimer++;

    // Frame -1: Deferred camera setup (Demise pattern)
    if (zpTimer == -1) {
        Camera_ChangeSetting(Play_GetCamera(play, 0), CAM_SET_TURN_AROUND);
        Camera_SetCameraData(Play_GetCamera(play, 0), 4, NULL, NULL, 10, 0, 0);
        p->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED;
    }

    // Lock player in place every frame
    p->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED;
    p->linearVelocity = 0.0f;
    p->actor.speedXZ = 0.0f;
    p->actor.velocity.x = p->actor.velocity.y = p->actor.velocity.z = 0.0f;

    // Frame 0: Start first animation
    if (zpTimer == 0) {
        LinkAnimation_Change(play, &p->skelAnime, &gPlayerAnim_link_magic_honoo1,
            ZPERM_ANIM_SPEED, 0.0f,
            Animation_GetLastFrame(&gPlayerAnim_link_magic_honoo1), ANIMMODE_ONCE, -8.0f);
        ZPerm_ComputePhaseEnd(zpTimer, Animation_GetLastFrame(&gPlayerAnim_link_magic_honoo1));
        Audio_PlaySoundGeneral(NA_SE_VO_LI_MAGIC_NALE, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }

    // Double-update: vanilla calls LinkAnimation_Update once, we call it again (Demise pattern)
    if (zpTimer >= 0) {
        LinkAnimation_Update(play, &p->skelAnime);
    }

    // Timer-based animation chaining (like Demise — does NOT rely on animDone)
    if (zpTimer > 0 && zpTimer >= sZPermPhaseEnd) {
        switch (zpSubPhase) {
            case ZPERM_CAST_HONOO1:
                LinkAnimation_Change(play, &p->skelAnime, &gPlayerAnim_link_magic_honoo2,
                    ZPERM_ANIM_SPEED, 0.0f,
                    Animation_GetLastFrame(&gPlayerAnim_link_magic_honoo2), ANIMMODE_ONCE, 0.0f);
                ZPerm_ComputePhaseEnd(zpTimer, Animation_GetLastFrame(&gPlayerAnim_link_magic_honoo2));
                zpSubPhase = ZPERM_CAST_HONOO2;
                break;

            case ZPERM_CAST_HONOO2:
                LinkAnimation_Change(play, &p->skelAnime, &gPlayerAnim_link_magic_honoo3,
                    ZPERM_ANIM_SPEED, 0.0f,
                    Animation_GetLastFrame(&gPlayerAnim_link_magic_honoo3), ANIMMODE_ONCE, 0.0f);
                ZPerm_ComputePhaseEnd(zpTimer, Animation_GetLastFrame(&gPlayerAnim_link_magic_honoo3));
                zpSubPhase = ZPERM_CAST_HONOO3;
                break;

            case ZPERM_CAST_HONOO3:
                // Casting complete — transition to ACTIVE
                // 1. Clear our custom blocking flags
                p->stateFlags1 &= ~(PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED);

                // 2. Reset player action to idle (handles action function,
                //    clears stateFlags, finishes anim movement — same as vanilla spell end)
                func_80839FFC(p, play);

                // 3. Reset camera
                func_8005B1A4(Play_GetCamera(play, 0));

                // Save time increment and freeze time
                zpSavedTime = gTimeIncrement;
                gTimeIncrement = 0;

                // Initial freeze on all targets
                ZPerm_FreezeAllTargets(play);

                zpState = ZPERM_STATE_ACTIVE;
                zpTimer = ZPERM_FREEZE_DURATION;

                // Green screen flash
                func_800AA000(300.0f, 150, 20, 60);

                // Freeze activation SFX
                Audio_PlaySoundGeneral(NA_SE_EV_ICE_FREEZE, &p->actor.world.pos, 4,
                    &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                break;
        }
    }

    // Green rune particles expanding outward during casting
    if (zpState == ZPERM_STATE_CASTING && zpTimer > 5 && play->gameplayFrames % 3 == 0) {
        f32 expandRadius = 20.0f + ((f32)(zpTimer - 5)) * 4.0f;
        if (expandRadius > 200.0f) expandRadius = 200.0f;
        ZPerm_SpawnCastRunes(p, play, expandRadius);
    }
}

// ============================================================================
// State: Active (freeze maintained, Link moves freely)
// ============================================================================

static void ZPerm_StateActive(Player* p, PlayState* play) {
    // Ensure Link stays free to move (defensive: clear in case anything re-sets these)
    p->stateFlags1 &= ~(PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED);

    // Re-freeze all targets every frame
    ZPerm_FreezeAllTargets(play);

    // Keep time frozen
    gTimeIncrement = 0;

    // Countdown
    zpTimer--;

    // Ambient frozen particles (flicker in last 40 frames)
    u8 flickering = (zpTimer <= ZPERM_FLICKER_START);
    if (!flickering || (play->gameplayFrames % 4 >= 2)) {
        ZPerm_SpawnFrozenParticles(p, play);
    }

    // Ambient SFX
    if (play->gameplayFrames % 40 == 0) {
        Audio_PlaySoundGeneral(NA_SE_EV_ICE_MELT, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }

    // Timer expired
    if (zpTimer <= 0) {
        zpState = ZPERM_STATE_ENDING;
        zpTimer = 0;
    }
}

// ============================================================================
// State: Ending (single cleanup frame)
// ============================================================================

static void ZPerm_StateEnding(Player* p, PlayState* play) {
    // Restore time
    gTimeIncrement = zpSavedTime;

    // Unfreeze all actors
    ZPerm_UnfreezeAllTargets(play);

    // Screen flash as time resumes
    func_800AA000(200.0f, 100, 15, 40);

    // Unfreeze SFX
    Audio_PlaySoundGeneral(NA_SE_EV_ICE_MELT, &p->actor.world.pos, 4,
        &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

    // Reset to idle
    zpActive = 0;
    zpState = ZPERM_STATE_IDLE;
    zpSubPhase = 0;
    zpTimer = 0;
    zpSavedTime = 0;
}

// ============================================================================
// Main Handler
// ============================================================================

void Handle_ZonaiPermafrost(Player* p, PlayState* play) {
    ItemInputState in;
    ItemInput_Update(&in, ITEM_ZONAI_PERMAFROST, p, play);

    // Unequipped: cleanup if active
    if (!in.wasEquipped) {
        if (zpActive) ZPerm_Stop(p, play);
        return;
    }

    // Damage cancels the spell during CASTING only (magic already consumed, non-cancellable by buttons)
    if (zpState == ZPERM_STATE_CASTING) {
        if (ItemInput_CheckDamage(p, &sZPermPrevInvinc)) {
            ZPerm_Stop(p, play);
            return;
        }
    }

    // Cannot use in water
    if (p->stateFlags1 & PLAYER_STATE1_IN_WATER) return;

    if (!zpActive) {
        if (ItemInput_IsBlocked(p, play)) return;
        if (in.isPressed) ZPerm_Start(p, play);
        return;
    }

    switch (zpState) {
        case ZPERM_STATE_CASTING: ZPerm_StateCasting(p, play); break;
        case ZPERM_STATE_ACTIVE:  ZPerm_StateActive(p, play); break;
        case ZPERM_STATE_ENDING:  ZPerm_StateEnding(p, play); break;
        default: ZPerm_Stop(p, play); break;
    }
}

// ============================================================================
// Stubs
// ============================================================================

void Player_InitZonaiPermafrostIA(PlayState* play, Player* p) {
    zpActive = 0;
    zpState = ZPERM_STATE_IDLE;
    zpSubPhase = 0;
    zpTimer = 0;
    zpSavedTime = 0;
}

s32 Player_UpperAction_ZonaiPermafrost(Player* p, PlayState* play) { return 0; }

/**
 * item_time_gate.c - Time Gate (custom time travel item)
 *
 * Controls:
 *   C Button: Activate time travel (requires 48 MP)
 *   Yes/No:   Confirm age swap
 *
 * Features:
 *   - Swaps Link between child and adult
 *   - Nayru's Love style cast animation
 *   - "Travel through time?" confirmation prompt
 *   - Magic only consumed on confirmation
 *   - Scene reloads on age change
 */

#include "z64.h"
#include "item_time_gate.h"
#include "../custom_items.h"
#include "../helpers/equip_helper.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"

// SwitchAge() is declared in mods.h with extern "C" linkage
extern void SwitchAge(void);

static s8 sTGPrevInvinc = 0;
static s32 sTGPhaseEnd = 0;  // Absolute tgTimer value when current anim phase ends

// Halved from vanilla 0.83f to compensate for double LinkAnimation_Update
// (vanilla's Player_Action_Idle calls it once, we call it again — Demise pattern).
// Effective speed per tick: 0.415 * R_UPDATE_RATE, matching vanilla's 0.83 * R * 0.5
#define TGATE_ANIM_SPEED 0.415f

// =============================================================================
// Phase end computation (accounts for R_UPDATE_RATE)
// =============================================================================

static void TGate_ComputePhaseEnd(s32 baseTimer, f32 lastFrame) {
    f32 rate = TGATE_ANIM_SPEED * R_UPDATE_RATE;
    if (rate < 0.1f) rate = 0.415f;  // Safety fallback
    sTGPhaseEnd = baseTimer + (s32)(lastFrame / rate) + 1;
}

// =============================================================================
// Stop / Start
// =============================================================================

static void TimeGate_Stop(Player* p, PlayState* play) {
    if (!tgActive) return;

    // Close any open textbox
    if (tgPromptShown) {
        Message_CloseTextbox(play);
        play->msgCtx.msgMode = MSGMODE_TEXT_DONE;
    }

    // Release player
    p->stateFlags1 &= ~(PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED);

    // Reset camera
    func_8005B1A4(Play_GetCamera(play, 0));

    // Reset state
    tgActive = 0;
    tgState = TGATE_STATE_IDLE;
    tgSubPhase = 0;
    tgTimer = 0;
    tgPromptShown = 0;
}

static void TimeGate_Start(Player* p, PlayState* play) {
    if (tgActive) return;

    // Validate magic (don't consume yet - only on Yes)
    if (!ItemMagic_HasEnough(play, TGATE_MAGIC_COST)) {
        Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        return;
    }

    // Must be on ground
    if (!(p->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) return;

    // Cannot use in water
    if (p->stateFlags1 & PLAYER_STATE1_IN_WATER) return;

    // Activate
    tgActive = 1;
    tgState = TGATE_STATE_CASTING;
    tgSubPhase = TGATE_CAST_TAMASHII1;
    tgTimer = -2;  // Deferred setup on frame -1
    tgPromptShown = 0;
    sTGPhaseEnd = 0;
}

// =============================================================================
// State: Casting (Nayru's Love animation sequence: tamashii1 -> tamashii2 -> tamashii3)
//
// Uses DEMISE PATTERN for reliable animation handling:
// 1. LinkAnimation_Change at 0.415f (half vanilla speed)
// 2. Explicit LinkAnimation_Update call (double-update with vanilla's call)
//    -> effective speed = 0.415 * R_UPDATE_RATE per tick = vanilla's 0.83 * R * 0.5
// 3. Timer-based chaining computed from R_UPDATE_RATE (never relies on animDone)
// =============================================================================

static void TimeGate_StateCasting(Player* p, PlayState* play) {
    tgTimer++;

    // Deferred setup on frame -1: camera + state flags
    if (tgTimer == -1) {
        Camera_ChangeSetting(Play_GetCamera(play, 0), CAM_SET_TURN_AROUND);
        Camera_SetCameraData(Play_GetCamera(play, 0), 4, NULL, NULL, 10, 0, 0);
        p->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED;
    }

    // Lock player in place every frame
    p->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED;
    p->linearVelocity = 0.0f;
    p->actor.speedXZ = 0.0f;
    p->actor.velocity.x = p->actor.velocity.y = p->actor.velocity.z = 0.0f;

    // Start first animation on frame 0
    if (tgTimer == 0) {
        LinkAnimation_Change(play, &p->skelAnime, &gPlayerAnim_link_magic_tamashii1, TGATE_ANIM_SPEED, 0.0f,
            Animation_GetLastFrame(&gPlayerAnim_link_magic_tamashii1), ANIMMODE_ONCE, -8.0f);
        TGate_ComputePhaseEnd(tgTimer, Animation_GetLastFrame(&gPlayerAnim_link_magic_tamashii1));
        Audio_PlaySoundGeneral(NA_SE_VO_LI_MAGIC_NALE, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }

    // Double-update: vanilla calls LinkAnimation_Update once, we call it again (Demise pattern)
    if (tgTimer >= 0) {
        LinkAnimation_Update(play, &p->skelAnime);
    }

    // Timer-based animation chaining (like Demise — does NOT rely on animDone)
    if (tgTimer > 0 && tgTimer >= sTGPhaseEnd) {
        switch (tgSubPhase) {
            case TGATE_CAST_TAMASHII1:
                LinkAnimation_Change(play, &p->skelAnime, &gPlayerAnim_link_magic_tamashii2,
                    TGATE_ANIM_SPEED, 0.0f,
                    Animation_GetLastFrame(&gPlayerAnim_link_magic_tamashii2), ANIMMODE_ONCE, 0.0f);
                TGate_ComputePhaseEnd(tgTimer, Animation_GetLastFrame(&gPlayerAnim_link_magic_tamashii2));
                tgSubPhase = TGATE_CAST_TAMASHII2;
                break;

            case TGATE_CAST_TAMASHII2:
                LinkAnimation_Change(play, &p->skelAnime, &gPlayerAnim_link_magic_tamashii3,
                    TGATE_ANIM_SPEED, 0.0f,
                    Animation_GetLastFrame(&gPlayerAnim_link_magic_tamashii3), ANIMMODE_ONCE, 0.0f);
                TGate_ComputePhaseEnd(tgTimer, Animation_GetLastFrame(&gPlayerAnim_link_magic_tamashii3));
                tgSubPhase = TGATE_CAST_TAMASHII3;
                break;

            case TGATE_CAST_TAMASHII3:
                // Casting complete - transition to hovering
                tgState = TGATE_STATE_HOVERING;
                tgTimer = 0;
                tgPromptShown = 0;
                break;
        }
    }

    // Blue-purple sparkles during casting (time-themed)
    if (tgTimer > 10 && play->gameplayFrames % 4 == 0) {
        Vec3f sparklePos = p->actor.world.pos;
        sparklePos.y += 30.0f + Rand_ZeroFloat(30.0f);
        sparklePos.x += Rand_CenteredFloat(20.0f);
        sparklePos.z += Rand_CenteredFloat(20.0f);
        Vec3f vel = { 0.0f, 1.5f, 0.0f };
        Vec3f accel = { 0.0f, 0.0f, 0.0f };
        Color_RGBA8 primColor = { 150, 150, 255, 255 };
        Color_RGBA8 envColor = { 80, 50, 200, 255 };
        EffectSsKiraKira_SpawnFocused(play, &sparklePos, &vel, &accel, &primColor, &envColor, 600, 20);
    }
}

// =============================================================================
// State: Hovering (warp animation + Yes/No textbox)
// Link floats in place while the prompt is displayed.
// =============================================================================

static void TimeGate_StateHovering(Player* p, PlayState* play) {
    // Lock player in place
    p->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED;
    p->linearVelocity = 0.0f;
    p->actor.speedXZ = 0.0f;
    p->actor.velocity.x = p->actor.velocity.y = p->actor.velocity.z = 0.0f;

    tgTimer++;

    // Play warp hover animation on entry
    if (tgTimer == 1) {
        LinkAnimation_Change(play, &p->skelAnime, &gPlayerAnim_link_demo_warp, TGATE_ANIM_SPEED, 0.0f,
            Animation_GetLastFrame(&gPlayerAnim_link_demo_warp), ANIMMODE_LOOP, -8.0f);
        Audio_PlaySoundGeneral(NA_SE_PL_MAGIC_WIND_WARP, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }

    // Double-update (Demise pattern)
    if (tgTimer >= 1) {
        LinkAnimation_Update(play, &p->skelAnime);
    }

    // Show textbox after settling into hover
    if (tgTimer == TGATE_HOVER_SETTLE && !tgPromptShown) {
        Message_StartTextbox(play, TEXT_TIME_GATE_PROMPT, NULL);
        tgPromptShown = 1;
    }

    // Blue-purple sparkles while hovering
    if (play->gameplayFrames % 3 == 0) {
        Vec3f sparklePos = p->actor.world.pos;
        sparklePos.y += 20.0f + Rand_ZeroFloat(40.0f);
        sparklePos.x += Rand_CenteredFloat(25.0f);
        sparklePos.z += Rand_CenteredFloat(25.0f);
        Vec3f vel = { 0.0f, 1.0f, 0.0f };
        Vec3f accel = { 0.0f, 0.0f, 0.0f };
        Color_RGBA8 primColor = { 150, 150, 255, 255 };
        Color_RGBA8 envColor = { 80, 50, 200, 255 };
        EffectSsKiraKira_SpawnFocused(play, &sparklePos, &vel, &accel, &primColor, &envColor, 400, 15);
    }

    // Poll for player choice
    if (tgPromptShown && Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) {
        if (Message_ShouldAdvance(play)) {
            // Close the textbox
            Message_CloseTextbox(play);
            play->msgCtx.msgMode = MSGMODE_TEXT_DONE;

            if (play->msgCtx.choiceIndex == 0) {
                // YES - switch age
                tgState = TGATE_STATE_SWITCHING;
                tgTimer = 0;
            } else {
                // NO - cancel
                tgState = TGATE_STATE_CANCEL;
                tgTimer = 0;
            }
        }
    }
}

// =============================================================================
// State: Switching (user chose Yes - consume magic and switch age)
// =============================================================================

static void TimeGate_StateSwitching(Player* p, PlayState* play) {
    // Consume magic now that user confirmed
    ItemMagic_Consume(play, TGATE_MAGIC_COST);

    // Screen flash effect
    func_800AA000(400.0f, 200, 30, 100);

    // Play transition sound
    Audio_PlaySoundGeneral(NA_SE_SY_WHITE_OUT_T, &p->actor.world.pos, 4,
        &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

    // Release player state before SwitchAge (it triggers scene reload)
    p->stateFlags1 &= ~(PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED);
    func_8005B1A4(Play_GetCamera(play, 0));

    // Reset our state (scene will reload anyway)
    tgActive = 0;
    tgState = TGATE_STATE_IDLE;
    tgSubPhase = 0;
    tgTimer = 0;
    tgPromptShown = 0;

    // Switch age - this triggers scene transition
    SwitchAge();
}

// =============================================================================
// State: Cancel (user chose No - exit animation, return control)
// =============================================================================

static void TimeGate_StateCancel(Player* p, PlayState* play) {
    p->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED;
    p->linearVelocity = 0.0f;
    p->actor.speedXZ = 0.0f;

    tgTimer++;

    // Play exit animation (tamashii3 = Nayru's Love descend)
    if (tgTimer == 1) {
        LinkAnimation_Change(play, &p->skelAnime, &gPlayerAnim_link_magic_tamashii3, TGATE_ANIM_SPEED, 0.0f,
            Animation_GetLastFrame(&gPlayerAnim_link_magic_tamashii3), ANIMMODE_ONCE, -8.0f);
    }

    // Double-update (Demise pattern)
    if (tgTimer >= 1) {
        LinkAnimation_Update(play, &p->skelAnime);
    }

    // End after cancel duration
    if (tgTimer >= TGATE_CANCEL_DURATION) {
        p->stateFlags1 &= ~(PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED);
        func_8005B1A4(Play_GetCamera(play, 0));

        tgActive = 0;
        tgState = TGATE_STATE_IDLE;
        tgSubPhase = 0;
        tgTimer = 0;
        tgPromptShown = 0;
    }
}

// =============================================================================
// Public API
// =============================================================================

void Handle_TimeGate(Player* p, PlayState* play) {
    ItemInputState in;
    ItemInput_Update(&in, ITEM_TIME_GATE, p, play);

    // Unequipped while active - stop
    if (!in.wasEquipped) {
        if (tgActive) TimeGate_Stop(p, play);
        return;
    }

    // Took damage while active - stop (skip terminal states)
    if (tgActive && tgState != TGATE_STATE_SWITCHING && tgState != TGATE_STATE_CANCEL) {
        if (ItemInput_CheckDamage(p, &sTGPrevInvinc)) {
            TimeGate_Stop(p, play);
            return;
        }
    }

    // Cannot use in water
    if (p->stateFlags1 & PLAYER_STATE1_IN_WATER) return;

    // Not active - check activation
    // otherButtonPressed only checked here (Hylia's Grace pattern).
    // When active, A/B are used by the textbox — must not cancel the spell.
    if (!tgActive) {
        if (in.otherButtonPressed) return;
        if (ItemInput_IsBlocked(p, play)) return;
        if (in.isPressed) TimeGate_Start(p, play);
        return;
    }

    // Dispatch to current state
    switch (tgState) {
        case TGATE_STATE_CASTING:   TimeGate_StateCasting(p, play); break;
        case TGATE_STATE_HOVERING:  TimeGate_StateHovering(p, play); break;
        case TGATE_STATE_SWITCHING: TimeGate_StateSwitching(p, play); break;
        case TGATE_STATE_CANCEL:    TimeGate_StateCancel(p, play); break;
        default: TimeGate_Stop(p, play); break;
    }
}

void Player_InitTimeGateIA(PlayState* play, Player* p) {
    if (tgActive) return;
    tgState = TGATE_STATE_IDLE;
    tgSubPhase = 0;
    tgTimer = 0;
    tgPromptShown = 0;
}

s32 Player_UpperAction_TimeGate(Player* p, PlayState* play) { return 0; }

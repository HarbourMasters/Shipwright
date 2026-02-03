/**
 * item_hylias_grace.c - Hylia's Grace (fairy transformation)
 *
 * Controls:
 *   C Button:     Activate (consumes magic)
 *   A (flying):   Ascend (drains timer faster)
 *   B (flying):   Descend
 *   L (flying):   Sprint (drains timer faster)
 *   Analog:       Flight direction
 *
 * Features:
 *   - Transform into fairy for 10 seconds
 *   - Free flight ignores collision
 *   - Green fairy glow visual effect
 *   - Farore's Wind style warp animation
 * vanilla's 0.83f) to compensate. Timer-based chaining via R_UPDATE_RATE
 * ensures reliable state transitions without relying on animDone.
 *
 * 1 minute cooldown after transformation ends
 */

#include "z64.h"
#include "item_hylias_grace.h"
#include "../custom_items.h"
#include "../helpers/equip_helper.h"
#include "../helpers/fx_helper.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "objects/gameplay_keep/gameplay_keep.h"

extern void Player_Draw(Actor* thisx, PlayState* play);

static s8 sHGracePrevInvinc = 0;
static s32 sHGPhaseEnd = 0;  // Absolute hgTimer value when current animation phase ends

// Saved fairy position — used to undo displacement from the normal collision
// response (Actor_UpdateBgCheckInfo / OC) that runs after our code each frame.
static Vec3f sFairyPos;
static u8    sFairyPosValid = 0;

// Camera yaw captured at fairy mode start — fixed reference for stick movement.
// The free camera (NORMAL0) orbits unpredictably so we lock the movement reference.
static s16   sFairyCamYaw = 0;

// Halved from vanilla 0.83f to compensate for double LinkAnimation_Update
// (vanilla's Player_Action_Idle calls it once, we call it again — Demise pattern).
// Effective speed per tick: 0.415 * R_UPDATE_RATE, matching vanilla's 0.83 * R * 0.5
#define HGRACE_ANIM_SPEED 0.415f

// =============================================================================
// Fairy visual effects
// =============================================================================

static void HGrace_SpawnFairySparkles(Player* p, PlayState* play) {
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Color_RGBA8 primColor = { 150, 255, 150, 255 };
    Color_RGBA8 envColor = { 50, 200, 50, 255 };

    for (u8 i = 0; i < 3; i++) {
        Vec3f pos;
        pos.x = p->actor.world.pos.x + Rand_CenteredFloat(15.0f);
        pos.y = p->actor.world.pos.y + 20.0f + Rand_CenteredFloat(10.0f);
        pos.z = p->actor.world.pos.z + Rand_CenteredFloat(15.0f);

        Vec3f vel;
        vel.x = Rand_CenteredFloat(1.0f);
        vel.y = Rand_ZeroFloat(1.0f);
        vel.z = Rand_CenteredFloat(1.0f);

        EffectSsKiraKira_SpawnFocused(play, &pos, &vel, &accel, &primColor, &envColor, 300, 12);
    }
}

static void HGrace_SpawnWarpSparkles(PlayState* play, Vec3f* center) {
    Vec3f pos, vel, accel;
    Color_RGBA8 primColor = { 100, 255, 150, 255 };
    Color_RGBA8 envColor = { 0, 200, 100, 255 };

    accel.x = accel.y = accel.z = 0.0f;

    for (u8 i = 0; i < 16; i++) {
        f32 angle = (f32)i * (65536.0f / 16.0f);
        s16 angleS = (s16)angle;

        pos.x = center->x + Math_SinS(angleS) * 30.0f;
        pos.y = center->y + 20.0f + Rand_ZeroFloat(20.0f);
        pos.z = center->z + Math_CosS(angleS) * 30.0f;

        vel.x = Math_SinS(angleS) * 2.0f;
        vel.y = Rand_ZeroFloat(3.0f) + 1.0f;
        vel.z = Math_CosS(angleS) * 2.0f;

        EffectSsKiraKira_SpawnFocused(play, &pos, &vel, &accel, &primColor, &envColor, 800, 25);
    }
}

// =============================================================================
// Fairy draw function (green fairy — replaces Link's model during fairy mode)
//
// Body: 3-layer glow circles (large dim + medium + bright core) using the
//       light-system pattern (dithering + gGlowCircleDL, billboarded).
// Wings: Vanilla gFairyWing1DL-4DL with segment 0x08 material setup matching
//        EnElf_Draw (Gfx_SetupDL_27Xlu, G_RM_ZB_CLD_SURF2, pulsating envAlpha).
// =============================================================================

static void HGrace_DrawFairy(Actor* thisx, PlayState* play) {
    Player* p = (Player*)thisx;

    // Flicker: skip drawing during flicker-off frames (last 1.5 seconds)
    if ((hgTimer <= HGRACE_FLICKER_START) && ((play->gameplayFrames % 6) < 3)) return;

    OPEN_DISPS(play->state.gfxCtx);

    f32 bx = p->actor.world.pos.x;
    f32 by = p->actor.world.pos.y + 20.0f;
    f32 bz = p->actor.world.pos.z;

    // Vanilla fairy pulsation: scale oscillates ±10% around 1.0
    f32 pulse = (Math_SinS(play->gameplayFrames * 4096) * 0.1f) + 1.0f;

    // ===== BODY GLOW (single layer, SETUPDL_65 + CLD_SURF) =====
    POLY_XLU_DISP = func_800947AC(POLY_XLU_DISP++);
    gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_NOISE);
    gDPSetColorDither(POLY_XLU_DISP++, G_CD_MAGICSQ);
    gSPDisplayList(POLY_XLU_DISP++, gGlowCircleTextureLoadDL);

    {
        f32 s = 0.035f * pulse;
        Matrix_Translate(bx, by, bz, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(s, s, s, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 180, 255, 180, 120);
        gSPDisplayList(POLY_XLU_DISP++, gGlowCircleDL);
    }

    // ===== INNER WHITE CORE (vanilla fairy inner glow) =====
    {
        f32 si = 0.012f * pulse;
        Matrix_Translate(bx, by, bz, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(si, si, si, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 200);
        gSPDisplayList(POLY_XLU_DISP++, gGlowCircleDL);
    }

    // ===== FAIRY WINGS (vanilla EnElf_Draw material pattern) =====
    Gfx_SetupDL_27Xlu(play->state.gfxCtx);

    // Segment 0x08: fairy material — PrimColor + RenderMode (matches EnElf_Draw)
    {
        Gfx* seg = Graph_Alloc(play->state.gfxCtx, sizeof(Gfx) * 4);
        gSPSegment(POLY_XLU_DISP++, 0x08, seg);
        gDPPipeSync(seg++);
        gDPSetPrimColor(seg++, 0, 0x01, 200, 255, 200, 180);
        gDPSetRenderMode(seg++, G_RM_PASS, G_RM_ZB_CLD_SURF2);
        gSPEndDisplayList(seg++);
    }

    // Outer color with pulsating alpha (vanilla fairy breathing effect)
    s32 envAlpha = (play->gameplayFrames * 50) & 0x1FF;
    envAlpha = (envAlpha > 255) ? 511 - envAlpha : envAlpha;
    gDPSetEnvColor(POLY_XLU_DISP++, 100, 255, 100, (u8)envAlpha);

    // Wing flap animation (sinusoidal, ~±40 degrees)
    f32 flapRad = Math_SinS(play->gameplayFrames * 5000) * 0.7f;
    f32 ws = 0.008f * pulse;
    f32 wsLower = ws * 0.8f;
    f32 yr = p->actor.shape.rot.y * (M_PI / 32768.0f);

    // Right upper wing
    Matrix_Translate(bx, by + 2.0f, bz, MTXMODE_NEW);
    Matrix_RotateY(yr, MTXMODE_APPLY);
    Matrix_Translate(5.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    Matrix_RotateZ(flapRad + 0.5f, MTXMODE_APPLY);
    Matrix_Scale(ws, ws, ws, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gFairyWing1DL);

    // Right lower wing
    Matrix_Translate(bx, by - 1.0f, bz, MTXMODE_NEW);
    Matrix_RotateY(yr, MTXMODE_APPLY);
    Matrix_Translate(4.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    Matrix_RotateZ(flapRad + 0.3f, MTXMODE_APPLY);
    Matrix_Scale(wsLower, wsLower, wsLower, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gFairyWing2DL);

    // Left upper wing (mirrored X scale to flip geometry)
    Matrix_Translate(bx, by + 2.0f, bz, MTXMODE_NEW);
    Matrix_RotateY(yr, MTXMODE_APPLY);
    Matrix_Translate(-5.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    Matrix_RotateZ(-flapRad - 0.5f, MTXMODE_APPLY);
    Matrix_Scale(-ws, ws, ws, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gFairyWing3DL);

    // Left lower wing (mirrored X scale to flip geometry)
    Matrix_Translate(bx, by - 1.0f, bz, MTXMODE_NEW);
    Matrix_RotateY(yr, MTXMODE_APPLY);
    Matrix_Translate(-4.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    Matrix_RotateZ(-flapRad - 0.3f, MTXMODE_APPLY);
    Matrix_Scale(-wsLower, wsLower, wsLower, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gFairyWing4DL);

    CLOSE_DISPS(play->state.gfxCtx);
}

// =============================================================================
// Stop / Start
// =============================================================================

static void HGrace_Stop(Player* p, PlayState* play) {
    if (!hgActive) return;

    // Restore Link
    p->actor.draw = Player_Draw;
    p->stateFlags1 &= ~(PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED);
    p->invincibilityTimer = 20;

    // Re-enable colliders
    p->cylinder.base.atFlags |= AT_ON;
    p->cylinder.base.acFlags |= AC_ON;
    p->cylinder.base.ocFlags1 |= OC1_ON;

    // Reset camera
    func_8005B1A4(Play_GetCamera(play, 0));

    // Only set cooldown if fairy mode was actually reached
    u8 wasFairy = (hgState == HGRACE_STATE_FAIRY || hgState == HGRACE_STATE_WARP_OUT);

    hgActive = 0;
    hgState = HGRACE_STATE_IDLE;
    hgSubPhase = 0;
    hgTimer = 0;
    hgFairy = NULL;
    sFairyPosValid = 0;

    if (wasFairy) {
        hgCooldown = HGRACE_COOLDOWN;
    }
}

static void HGrace_Start(Player* p, PlayState* play) {
    if (hgActive) return;
    if (!ItemMagic_HasEnough(play, HGRACE_MAGIC_COST)) {
        Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        return;
    }
    if (!(p->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) return;
    if (p->stateFlags1 & PLAYER_STATE1_IN_WATER) return;

    // Requires a fairy in a bottle — consumed on cast (bottle becomes empty)
    if (!Inventory_ConsumeFairy(play)) {
        Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        return;
    }

    hgActive = 1;
    hgState = HGRACE_STATE_CASTING;
    hgSubPhase = HGRACE_CAST_KAZE1;
    hgTimer = -2;
    ItemMagic_Consume(play, HGRACE_MAGIC_COST);
}

// =============================================================================
// State: Casting (Farore's Wind animation sequence)
//
// Uses DEMISE PATTERN for reliable animation handling:
// 1. LinkAnimation_Change at 0.415f (half vanilla speed)
// 2. Explicit LinkAnimation_Update call (double-update with vanilla's call)
//    → effective speed = 0.415 * R_UPDATE_RATE per tick = vanilla's 0.83 * R * 0.5
// 3. Timer-based chaining computed from R_UPDATE_RATE (never relies on animDone)
// =============================================================================

static void HGrace_ComputePhaseEnd(s32 baseTimer, f32 lastFrame) {
    f32 rate = HGRACE_ANIM_SPEED * R_UPDATE_RATE;
    if (rate < 0.1f) rate = 0.415f;  // Safety fallback
    sHGPhaseEnd = baseTimer + (s32)(lastFrame / rate) + 1;
}

static void HGrace_StateCasting(Player* p, PlayState* play) {
    hgTimer++;

    // Deferred setup (Demise pattern): camera + stateFlags on first tick
    if (hgTimer == -1) {
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
    if (hgTimer == 0) {
        LinkAnimation_Change(play, &p->skelAnime, &gPlayerAnim_link_magic_kaze1, HGRACE_ANIM_SPEED, 0.0f,
            Animation_GetLastFrame(&gPlayerAnim_link_magic_kaze1), ANIMMODE_ONCE, -8.0f);
        HGrace_ComputePhaseEnd(hgTimer, Animation_GetLastFrame(&gPlayerAnim_link_magic_kaze1));
        Audio_PlaySoundGeneral(NA_SE_VO_LI_MAGIC_FROL, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }

    // Double-update: vanilla calls LinkAnimation_Update once, we call it again (Demise pattern)
    if (hgTimer >= 0) {
        LinkAnimation_Update(play, &p->skelAnime);
    }

    // Timer-based animation chaining (like Demise — does NOT rely on animDone)
    if (hgTimer > 0 && hgTimer >= sHGPhaseEnd) {
        switch (hgSubPhase) {
            case HGRACE_CAST_KAZE1:
                LinkAnimation_Change(play, &p->skelAnime, &gPlayerAnim_link_magic_kaze2,
                    HGRACE_ANIM_SPEED, 0.0f,
                    Animation_GetLastFrame(&gPlayerAnim_link_magic_kaze2), ANIMMODE_ONCE, 0.0f);
                HGrace_ComputePhaseEnd(hgTimer, Animation_GetLastFrame(&gPlayerAnim_link_magic_kaze2));
                hgSubPhase = HGRACE_CAST_KAZE2;
                break;

            case HGRACE_CAST_KAZE2:
                LinkAnimation_Change(play, &p->skelAnime, &gPlayerAnim_link_magic_kaze3,
                    HGRACE_ANIM_SPEED, 0.0f,
                    Animation_GetLastFrame(&gPlayerAnim_link_magic_kaze3), ANIMMODE_ONCE, 0.0f);
                HGrace_ComputePhaseEnd(hgTimer, Animation_GetLastFrame(&gPlayerAnim_link_magic_kaze3));
                hgSubPhase = HGRACE_CAST_KAZE3;
                break;

            case HGRACE_CAST_KAZE3:
                // Casting complete — transition to warp
                hgState = HGRACE_STATE_WARP_IN;
                hgTimer = 0;
                break;
        }
    }

    // Green sparkles during casting
    if (hgTimer > 10 && play->gameplayFrames % 4 == 0) {
        Vec3f sparklePos = p->actor.world.pos;
        sparklePos.y += 30.0f + Rand_ZeroFloat(30.0f);
        sparklePos.x += Rand_CenteredFloat(20.0f);
        sparklePos.z += Rand_CenteredFloat(20.0f);
        Vec3f vel = { 0.0f, 1.5f, 0.0f };
        Vec3f accel = { 0.0f, 0.0f, 0.0f };
        Color_RGBA8 primColor = { 150, 255, 150, 255 };
        Color_RGBA8 envColor = { 50, 200, 50, 255 };
        EffectSsKiraKira_SpawnFocused(play, &sparklePos, &vel, &accel, &primColor, &envColor, 600, 20);
    }
}

// =============================================================================
// State: Warp Enter (blue warp transition)
// =============================================================================

static void HGrace_StateWarpEnter(Player* p, PlayState* play) {
    p->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED;
    p->linearVelocity = 0.0f;
    p->actor.speedXZ = 0.0f;

    hgTimer++;

    if (hgTimer == 1) {
        LinkAnimation_Change(play, &p->skelAnime, &gPlayerAnim_link_demo_warp, HGRACE_ANIM_SPEED, 0.0f,
            Animation_GetLastFrame(&gPlayerAnim_link_demo_warp), ANIMMODE_ONCE, -8.0f);
        Audio_PlaySoundGeneral(NA_SE_PL_MAGIC_WIND_WARP, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }

    // Double-update (Demise pattern)
    if (hgTimer >= 1) {
        LinkAnimation_Update(play, &p->skelAnime);
    }

    // Blue-green warp sparkles
    if (hgTimer % 3 == 0) {
        Vec3f sparklePos = p->actor.world.pos;
        sparklePos.y += 40.0f;
        sparklePos.x += Rand_CenteredFloat(30.0f);
        sparklePos.z += Rand_CenteredFloat(30.0f);
        Vec3f vel = { 0.0f, 2.0f, 0.0f };
        Vec3f accel = { 0.0f, -0.1f, 0.0f };
        Color_RGBA8 primColor = { 100, 255, 200, 255 };
        Color_RGBA8 envColor = { 0, 150, 255, 255 };
        EffectSsKiraKira_SpawnFocused(play, &sparklePos, &vel, &accel, &primColor, &envColor, 600, 20);
    }

    // Screen flash at midpoint
    if (hgTimer == HGRACE_WARP_IN_DURATION / 2) {
        func_800AA000(400.0f, 200, 30, 100);
    }

    // Transition to fairy mode — draw fairy DL instead of Link
    if (hgTimer >= HGRACE_WARP_IN_DURATION) {
        p->actor.draw = HGrace_DrawFairy;
        p->invincibilityTimer = -1;

        // Disable all colliders
        p->cylinder.base.atFlags &= ~AT_ON;
        p->cylinder.base.acFlags &= ~AC_ON;
        p->cylinder.base.ocFlags1 &= ~OC1_ON;

        // Release camera so it follows the fairy during flight
        p->stateFlags1 &= ~PLAYER_STATE1_IN_ITEM_CS;
        func_8005B1A4(Play_GetCamera(play, 0));

        // Lock movement reference yaw from the current camera direction
        {
            Camera* cam = Play_GetCamera(play, 0);
            sFairyCamYaw = Math_Atan2S(cam->at.x - cam->eye.x, cam->at.z - cam->eye.z);
        }

        hgState = HGRACE_STATE_FAIRY;
        hgTimer = HGRACE_FAIRY_DURATION;
    }
}

// =============================================================================
// Fairy collision bypass
// Scene geometry always blocks. DynaPoly actors are passable UNLESS they are
// doors (listed in sHGraceDoorActors in the .h).
// =============================================================================

static s32 HGrace_IsPassableBarrier(PlayState* play, s32 bgId) {
    // Scene collision is never passable
    if (!DynaPoly_IsBgIdBgActor(bgId)) return 0;

    DynaPolyActor* dyna = DynaPoly_GetActor(&play->colCtx, bgId);
    if (dyna == NULL) return 0;

    // Doors block the fairy — everything else is passable
    for (s32 i = 0; i < (s32)ARRAY_COUNT(sHGraceDoorActors); i++) {
        if (dyna->actor.id == sHGraceDoorActors[i]) return 0;
    }
    return 1;
}

// =============================================================================
// State: Fairy (free flight mode)
// Link IS the fairy — fairy DL drawn, direct position control.
// Joystick = XZ movement (camera-relative), A = ascend (+4), B = descend (-4)
// L = sprint (2x speed, 2x timer drain)
// =============================================================================

static void HGrace_StateFairy(Player* p, PlayState* play) {
    // Restore position: the normal player update (Actor_UpdateBgCheckInfo)
    // runs after our code and may push world.pos out of DynaPoly actors.
    // By snapping back to the position we computed last frame, that
    // displacement is undone and our BgCheck bypass stays authoritative.
    if (sFairyPosValid) {
        p->actor.world.pos = sFairyPos;
    }

    // INPUT_DISABLED prevents normal player actions; IN_ITEM_CS is NOT set
    // so the camera follows the fairy freely (stick-relative movement works).
    p->stateFlags1 |= PLAYER_STATE1_INPUT_DISABLED;
    p->stateFlags1 &= ~PLAYER_STATE1_IN_ITEM_CS;
    p->actor.draw = HGrace_DrawFairy;
    p->invincibilityTimer = -1;

    // Zero ALL velocity — we move position directly
    p->linearVelocity = 0.0f;
    p->actor.speedXZ = 0.0f;
    p->actor.velocity.x = 0.0f;
    p->actor.velocity.y = 0.0f;
    p->actor.velocity.z = 0.0f;

    // Disable colliders every frame
    p->cylinder.base.atFlags &= ~AT_ON;
    p->cylinder.base.acFlags &= ~AC_ON;
    p->cylinder.base.ocFlags1 &= ~OC1_ON;

    // Read input (A/B for vertical, stick for horizontal, L for sprint)
    u8 aBtn = CHECK_BTN_ALL(play->state.input[0].cur.button, BTN_A);
    u8 bBtn = CHECK_BTN_ALL(play->state.input[0].cur.button, BTN_B);
    u8 lBtn = CHECK_BTN_ALL(play->state.input[0].cur.button, BTN_L);
    s8 stickX = play->state.input[0].cur.stick_x;
    s8 stickY = play->state.input[0].cur.stick_y;

    // Base speed
    f32 speed = HGRACE_SPEED;
    if (lBtn) speed *= HGRACE_SPRINT_MULT;

    // Vertical: A = ascend, B = descend
    f32 velY = 0.0f;
    if (aBtn) velY = speed;
    if (bBtn) velY = -speed;

    // Horizontal: joystick camera-relative
    f32 velX = 0.0f, velZ = 0.0f;
    f32 stickMag = sqrtf(SQ((f32)stickX) + SQ((f32)stickY));

    if (stickMag > 10.0f) {
        // Use the fixed reference yaw captured at fairy mode start.
        // Reading the live camera would create a feedback loop because NORMAL0
        // orbits behind rot.y and the angle drifts unpredictably.
        s16 stickAngle = Math_Atan2S((f32)stickX, (f32)stickY);
        s16 moveYaw = sFairyCamYaw + stickAngle;

        f32 normMag = stickMag / 127.0f;
        if (normMag > 1.0f) normMag = 1.0f;

        velX = Math_SinS(moveYaw) * speed * normMag;
        velZ = Math_CosS(moveYaw) * speed * normMag;
    }

    // Move position directly with geometry collision
    Vec3f prevPos = p->actor.world.pos;
    Vec3f desiredPos;
    desiredPos.x = prevPos.x + velX;
    desiredPos.y = prevPos.y + velY;
    desiredPos.z = prevPos.z + velZ;

    // Line test: prevent passing through walls/floors/ceilings
    // (passable barriers like iron bars and bombable walls are ignored)
    CollisionPoly* hitPoly = NULL;
    s32 hitBgId = 0;
    Vec3f hitPoint;
    if (BgCheck_EntityLineTest1(&play->colCtx, &prevPos, &desiredPos, &hitPoint,
            &hitPoly, true, true, true, true, &hitBgId)) {
        if (!HGrace_IsPassableBarrier(play, hitBgId)) {
            desiredPos = hitPoint;
        }
    }

    // Floor constraint: don't go below ground
    // (passable barriers are ignored so fairy can fly through grates/floors)
    CollisionPoly* floorPoly = NULL;
    s32 floorBgId;
    f32 floor = BgCheck_EntityRaycastFloor5(play, &play->colCtx, &floorPoly, &floorBgId,
                                             &p->actor, &desiredPos);
    if (floor > BGCHECK_Y_MIN && desiredPos.y < floor + HGRACE_FAIRY_HOVER) {
        if (!HGrace_IsPassableBarrier(play, floorBgId)) {
            desiredPos.y = floor + HGRACE_FAIRY_HOVER;
        }
    }

    p->actor.world.pos = desiredPos;

    // Save authoritative position — next frame we restore it before computing
    // new movement, undoing any displacement the normal collision system applied.
    sFairyPos = desiredPos;
    sFairyPosValid = 1;

    // Fairy sparkles (trailing particles in addition to the glow DL)
    u8 flickering = (hgTimer <= HGRACE_FLICKER_START);
    u8 flickerVisible = !flickering || ((play->gameplayFrames % 6) >= 3);

    if (flickerVisible) {
        HGrace_SpawnFairySparkles(p, play);
    }

    // Timer countdown (2x drain when ascending or sprinting)
    s16 drain = 1;
    if (aBtn || lBtn) drain = 2;
    hgTimer -= drain;
    if (hgTimer < 0) hgTimer = 0;

    // Timer expired: end fairy mode
    if (hgTimer <= 0) {
        hgState = HGRACE_STATE_WARP_OUT;
        hgTimer = 0;
    }
}

// =============================================================================
// State: Warp Exit (Link reappears)
// =============================================================================

static void HGrace_StateWarpExit(Player* p, PlayState* play) {
    p->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED;
    p->linearVelocity = 0.0f;
    p->actor.speedXZ = 0.0f;

    hgTimer++;

    if (hgTimer == 1) {
        // Snap Link to the fairy's last authoritative position
        // (Actor_UpdateBgCheckInfo may have displaced world.pos since the
        // last fairy frame, so we restore from the saved position)
        if (sFairyPosValid) {
            p->actor.world.pos = sFairyPos;
            sFairyPosValid = 0;
        }

        // Restore Link visibility
        p->actor.draw = Player_Draw;

        // Re-enable colliders
        p->cylinder.base.atFlags |= AT_ON;
        p->cylinder.base.acFlags |= AC_ON;
        p->cylinder.base.ocFlags1 |= OC1_ON;

        // Snap to floor if close enough
        CollisionPoly* floorPoly = NULL;
        s32 bgId;
        f32 floor = BgCheck_EntityRaycastFloor5(play, &play->colCtx, &floorPoly, &bgId,
                                                 &p->actor, &p->actor.world.pos);
        if (floor > BGCHECK_Y_MIN && (p->actor.world.pos.y - floor) < 100.0f) {
            p->actor.world.pos.y = floor;
            p->actor.bgCheckFlags |= BGCHECKFLAG_GROUND;
        }

        HGrace_SpawnWarpSparkles(play, &p->actor.world.pos);
        Audio_PlaySoundGeneral(NA_SE_PL_MAGIC_WIND_WARP, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        func_800AA000(200.0f, 150, 20, 80);
    }

    // Brief exit transition
    if (hgTimer >= HGRACE_WARP_OUT_DURATION) {
        p->stateFlags1 &= ~(PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED);
        p->invincibilityTimer = 20;
        func_8005B1A4(Play_GetCamera(play, 0));

        hgActive = 0;
        hgState = HGRACE_STATE_IDLE;
        hgCooldown = HGRACE_COOLDOWN;
    }
}

// =============================================================================
// Public API
// =============================================================================

void Handle_HyliasGrace(Player* p, PlayState* play) {
    ItemInputState in;
    ItemInput_Update(&in, ITEM_HYLIAS_GRACE, p, play);

    if (!in.wasEquipped) { if (hgActive) HGrace_Stop(p, play); return; }
    if (ItemInput_CheckDamage(p, &sHGracePrevInvinc)) { HGrace_Stop(p, play); return; }

    // Cannot use in water
    if (p->stateFlags1 & PLAYER_STATE1_IN_WATER) return;

    if (!hgActive) {
        // Only block on otherButtonPressed when NOT active.
        // During fairy mode, A/B are used for ascend/descend — must not cancel spell.
        if (in.otherButtonPressed) return;
        if (ItemInput_IsBlocked(p, play)) return;
        if (hgCooldown > 0) return;
        if (in.isPressed) HGrace_Start(p, play);
        return;
    }

    switch (hgState) {
        case HGRACE_STATE_CASTING:  HGrace_StateCasting(p, play); break;
        case HGRACE_STATE_WARP_IN:  HGrace_StateWarpEnter(p, play); break;
        case HGRACE_STATE_FAIRY:    HGrace_StateFairy(p, play); break;
        case HGRACE_STATE_WARP_OUT: HGrace_StateWarpExit(p, play); break;
        default: HGrace_Stop(p, play); break;
    }
}

void Player_InitHyliasGraceIA(PlayState* play, Player* p) {
    if (hgActive) return;
    hgState = HGRACE_STATE_IDLE;
    hgSubPhase = 0;
    hgTimer = 0;
    hgFairy = NULL;
}

s32 Player_UpperAction_HyliasGrace(Player* p, PlayState* play) { return 0; }

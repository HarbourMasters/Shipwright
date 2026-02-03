/**
 * item_desire_sensor.c - Desire Sensor (randomizer utility)
 *
 * Controls:
 *   C Button: Activate sensor (costs 3 hearts)
 *
 * Features:
 *   - Reveals if current scene has uncollected major items (randomizer only)
 *   - Golden sparkles + chime if major item found
 *   - Ganondorf laugh + dark flash if not
 *   - ~4.5 second cutscene (sensing + result)
 */

#include "z64.h"
#include "item_desire_sensor.h"
#include "../custom_items.h"
#include "../helpers/equip_helper.h"
#include "../helpers/fx_helper.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"

static s8 sDSPrevInvinc = 0;

// Forward declare the C++ bridge function
u8 Randomizer_SceneHasMajorItem(s16 sceneNum);

// =============================================================================
// Visual Effects
// =============================================================================

// Mystical purple sparkles during sensing phase
static void DS_SpawnSensingSparkles(Player* p, PlayState* play) {
    Vec3f accel = { 0.0f, 0.05f, 0.0f };
    Color_RGBA8 primColor = { 180, 120, 255, 255 };
    Color_RGBA8 envColor  = { 80, 40, 200, 255 };

    for (u8 i = 0; i < 3; i++) {
        s16 angle = (s16)(Rand_ZeroOne() * 0xFFFF);
        f32 dist = 15.0f + Rand_ZeroOne() * 25.0f;

        Vec3f pos;
        pos.x = p->actor.world.pos.x + Math_SinS(angle) * dist;
        pos.y = p->actor.world.pos.y + 20.0f + Rand_CenteredFloat(40.0f);
        pos.z = p->actor.world.pos.z + Math_CosS(angle) * dist;

        Vec3f vel;
        vel.x = Math_SinS(angle) * 0.3f;
        vel.y = 1.5f + Rand_ZeroOne() * 1.0f;
        vel.z = Math_CosS(angle) * 0.3f;

        EffectSsKiraKira_SpawnFocused(play, &pos, &vel, &accel,
            &primColor, &envColor, 500, 18);
    }
}

// Golden sparkle burst - major item found
static void DS_SpawnGoldenBurst(Player* p, PlayState* play) {
    Color_RGBA8 primColor = { 255, 255, 100, 255 };
    Color_RGBA8 envColor  = { 255, 200, 0, 255 };

    for (u8 i = 0; i < 16; i++) {
        s16 angle = (s16)(Rand_ZeroOne() * 0xFFFF);
        f32 dist = 5.0f + Rand_ZeroOne() * 40.0f;

        Vec3f pos;
        pos.x = p->actor.world.pos.x + Math_SinS(angle) * dist;
        pos.y = p->actor.world.pos.y + 30.0f + Rand_CenteredFloat(30.0f);
        pos.z = p->actor.world.pos.z + Math_CosS(angle) * dist;

        Vec3f vel;
        vel.x = Math_SinS(angle) * 3.0f;
        vel.y = 2.0f + Rand_ZeroOne() * 4.0f;
        vel.z = Math_CosS(angle) * 3.0f;

        Vec3f accel = { 0.0f, -0.1f, 0.0f };

        EffectSsKiraKira_SpawnFocused(play, &pos, &vel, &accel,
            &primColor, &envColor, 1000, 30);
    }
}

// Dark red flash - no major item
static void DS_SpawnDarkFlash(Player* p, PlayState* play) {
    Color_RGBA8 primColor = { 200, 50, 50, 255 };
    Color_RGBA8 envColor  = { 100, 0, 0, 255 };

    for (u8 i = 0; i < 8; i++) {
        s16 angle = (s16)(Rand_ZeroOne() * 0xFFFF);
        f32 dist = 10.0f + Rand_ZeroOne() * 20.0f;

        Vec3f pos;
        pos.x = p->actor.world.pos.x + Math_SinS(angle) * dist;
        pos.y = p->actor.world.pos.y + 20.0f + Rand_CenteredFloat(20.0f);
        pos.z = p->actor.world.pos.z + Math_CosS(angle) * dist;

        Vec3f vel;
        vel.x = Math_SinS(angle) * 1.0f;
        vel.y = -0.5f;
        vel.z = Math_CosS(angle) * 1.0f;

        Vec3f accel = { 0.0f, -0.15f, 0.0f };

        EffectSsKiraKira_SpawnFocused(play, &pos, &vel, &accel,
            &primColor, &envColor, 400, 15);
    }
}

// =============================================================================
// Stop / Start
// =============================================================================

static void DS_Stop(Player* p, PlayState* play) {
    if (!dsActive) return;

    p->stateFlags1 &= ~(PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED);
    func_8005B1A4(Play_GetCamera(play, 0));

    dsActive = 0;
    dsState = DSENSOR_STATE_IDLE;
    dsTimer = 0;
    dsResult = 0;
}

static void DS_Start(Player* p, PlayState* play) {
    if (dsActive) return;

    // Must be in randomizer mode
    if (!IS_RANDO) {
        Audio_PlaySoundGeneral(DSENSOR_SE_ERROR, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
            &gSfxDefaultReverb);
        return;
    }

    // Health check: must have more than 3 hearts to survive
    if (gSaveContext.health <= DSENSOR_HEALTH_COST) {
        Audio_PlaySoundGeneral(DSENSOR_SE_ERROR, &p->actor.world.pos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
            &gSfxDefaultReverb);
        return;
    }

    // Must be on the ground
    if (!(p->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) return;

    // Pay health cost
    gSaveContext.health -= DSENSOR_HEALTH_COST;

    // Query the randomizer (result cached for the duration of the cutscene)
    dsResult = Randomizer_SceneHasMajorItem((s16)play->sceneNum);

    dsActive = 1;
    dsState = DSENSOR_STATE_SENSING;
    dsTimer = -2;  // Demise pattern: start at -2 for deferred camera setup
}

// =============================================================================
// State: SENSING (mini-cutscene, ~2.5 seconds)
// =============================================================================

static void DS_StateSensing(Player* p, PlayState* play) {
    dsTimer++;

    // Frame -1: Camera setup + lock player (Demise pattern deferred setup)
    if (dsTimer == -1) {
        Camera_ChangeSetting(Play_GetCamera(play, 0), CAM_SET_TURN_AROUND);
        Camera_SetCameraData(Play_GetCamera(play, 0), 4, NULL, NULL, 10, 0, 0);
        p->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED;
    }

    // Lock player every frame
    p->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED;
    p->linearVelocity = 0.0f;
    p->actor.speedXZ = 0.0f;

    // Spawn sensing sparkles (increasing density over time)
    if (dsTimer >= 5) {
        u8 interval = (dsTimer < 25) ? 4 : 2;
        if (dsTimer % interval == 0) {
            DS_SpawnSensingSparkles(p, play);
        }
    }

    // Subtle screen rumble in the last third
    if (dsTimer > 30 && dsTimer % 6 == 0) {
        func_800AA000(50.0f, 80, 8, 4);
    }

    // End of sensing phase -> reveal result
    if (dsTimer >= DSENSOR_SENSING_DURATION) {
        dsState = DSENSOR_STATE_RESULT;
        dsTimer = 0;
    }
}

// =============================================================================
// State: RESULT (reveal result, ~2 seconds hold)
// =============================================================================

static void DS_StateResult(Player* p, PlayState* play) {
    dsTimer++;

    // Keep player locked
    p->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_INPUT_DISABLED;
    p->linearVelocity = 0.0f;
    p->actor.speedXZ = 0.0f;

    // Frame 1: Play result sound + spawn result VFX
    if (dsTimer == 1) {
        if (dsResult) {
            // MAJOR ITEM FOUND
            DS_SpawnGoldenBurst(p, play);
            Audio_PlaySoundGeneral(DSENSOR_SE_FOUND, &gSfxDefaultPos, 4,
                &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                &gSfxDefaultReverb);
            func_800AA000(300.0f, 255, 40, 80);
        } else {
            // NO MAJOR ITEM
            DS_SpawnDarkFlash(p, play);
            Audio_PlaySoundGeneral(DSENSOR_SE_NOTHING, &gSfxDefaultPos, 4,
                &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                &gSfxDefaultReverb);
            func_800AA000(100.0f, 100, 10, 20);
        }
    }

    // Sustained golden sparkles for "found" result
    if (dsResult && dsTimer > 5 && dsTimer <= 20 && dsTimer % 4 == 0) {
        DS_SpawnGoldenBurst(p, play);
    }

    // End of result phase -> return to idle
    if (dsTimer >= DSENSOR_RESULT_DURATION) {
        DS_Stop(p, play);
    }
}

// =============================================================================
// Public API
// =============================================================================

void Player_InitDesireSensorIA(PlayState* play, Player* p) {
    dsActive = 0;
    dsState = DSENSOR_STATE_IDLE;
    dsTimer = 0;
    dsResult = 0;
}

void Handle_DesireSensor(Player* p, PlayState* play) {
    ItemInputState in;
    ItemInput_Update(&in, ITEM_DESIRE_SENSOR, p, play);

    // Unequipped while active -> abort
    if (!in.wasEquipped) {
        if (dsActive) DS_Stop(p, play);
        return;
    }

    // Damage or other button while active -> abort
    if (dsActive) {
        if (ItemInput_CheckDamage(p, &sDSPrevInvinc)) { DS_Stop(p, play); return; }
        if (in.otherButtonPressed) { DS_Stop(p, play); return; }
    }

    // Idle: wait for input
    if (!dsActive) {
        if (ItemInput_IsBlocked(p, play)) return;
        if (in.isPressed) DS_Start(p, play);
        return;
    }

    // Active: run state machine
    switch (dsState) {
        case DSENSOR_STATE_SENSING: DS_StateSensing(p, play); break;
        case DSENSOR_STATE_RESULT:  DS_StateResult(p, play);  break;
        default: DS_Stop(p, play); break;
    }
}

s32 Player_UpperAction_DesireSensor(Player* p, PlayState* play) { return 0; }

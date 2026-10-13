/*
 * File: z_rumble.c
 * Description: Rumble request system
 *
 * Provides a simple interface to allow scheduling up to RUMBLE_REQUEST_BUFFER_SIZE rumble requests to the RumblePak.
 * There's an additional Override type of rumble request for requests which should take priorities over any other
 * scheduled request.
 */

#include "global.h"
#include "z64rumble.h"

RumbleManager gRumbleMgr;

#define RUMBLE_FRAMES_PER_UPDATE (R_UPDATE_RATE > 0 ? R_UPDATE_RATE : 1)

void Rumble_Update(void* arg0) {
    // BENTODO: Workaround for rumble being too long.
    // On hardware, PadMgr_ThreadEntry would run once per VI (60hertz),
    // however we do not have OS Threads implemented, and instead opted to execute the PadMgr alongside
    // the Graph thread. This means it will run less often when the game is in 20fps or 30fps mode.
    // Here we call the rumble manager extra times to simulate updates happening at 60hertz.
    for (int i = 0; i < RUMBLE_FRAMES_PER_UPDATE; i++) {
        RumbleManager_Update(&gRumbleMgr);
        PadMgr_RumbleSet(gRumbleMgr.rumbleEnabled);
    }
}

// Used by some bosses (and fishing)
void Rumble_Override(f32 distSq, u8 sourceIntensity, u8 decayTimer, u8 decayStep) {
    s32 intensity;
    s32 distance;

    if (SQ(1000.0f) < distSq) {
        distance = 1000;
    } else {
        distance = sqrtf(distSq);
    }

    if ((distance < 1000) && (sourceIntensity != 0) && (decayStep != 0)) {
        intensity = sourceIntensity - (distance * 255) / 1000;

        if (intensity > 0) {
            gRumbleMgr.overrideIntensity = intensity;
            gRumbleMgr.overrideDecayTimer = decayTimer;
            gRumbleMgr.overrideDecayStep = decayStep;
        }
    }
}

void Rumble_Request(f32 distSq, u8 sourceIntensity, u8 decayTimer, u8 decayStep) {
    s32 intensity;
    s32 distance;
    s32 i;

    if (SQ(1000.0f) < distSq) {
        distance = 1000;
    } else {
        distance = sqrtf(distSq);
    }

    if ((distance < 1000) && (sourceIntensity != 0) && (decayStep != 0)) {
        intensity = sourceIntensity - (distance * 255) / 1000;

        for (i = 0; i < RUMBLE_REQUEST_BUFFER_SIZE; i++) {
            if (gRumbleMgr.requestIntensities[i] == 0) {
                if (intensity > 0) {
                    gRumbleMgr.requestIntensities[i] = intensity;
                    gRumbleMgr.requestDecayTimers[i] = decayTimer;
                    gRumbleMgr.requestDecaySteps[i] = decayStep;
                }
                break;
            }
        }
    }
}

void Rumble_Init(void) {
    RumbleManager_Init(&gRumbleMgr);
    PadMgr_SetRumbleRetraceCallback(Rumble_Update, NULL);
}

void Rumble_Destroy(void) {
    PadMgr_UnsetRumbleRetraceCallback(Rumble_Update, NULL);
    RumbleManager_Destroy(&gRumbleMgr);
}

s32 Rumble_ControllerOneHasRumblePak(void) {
    return PadMgr_ControllerHasRumblePak(0);
}

/**
 * Wipes every old request for a fresh start, then proceeds to process them as normal
 */
void Rumble_StateReset(void) {
    gRumbleMgr.state = RUMBLEMANAGER_STATE_INITIAL;
}

/**
 * Changes the state of the manager to WIPE
 *
 * In this state, every request is deleted
 */
void Rumble_StateWipeRequests(void) {
    gRumbleMgr.state = RUMBLEMANAGER_STATE_WIPE;
}

/**
 * Request processing is paused if updateEnabled is set to false
 */
void Rumble_SetUpdateEnabled(s32 updateEnabled) {
    gRumbleMgr.updateEnabled = !!updateEnabled;
}

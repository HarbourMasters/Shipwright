/*
 * File: sys_rumble.c
 * Description: Internal scheduler system for rumble requests
 */

#include "global.h"
#include "z64rumble.h"
#include <string.h>

// sRumbleWasEnabledOnLastTick/sWillDisableRumble? Probably name it after updateEnabled
u8 D_801D1E70 = true;

void RumbleManager_Update(RumbleManager* rumbleMgr) {
    s32 strongestIndex = -1;
    s32 i;
    s32 temp;

    // Turn rumbling off for all controllers
    for (i = 0; i < ARRAY_COUNT(rumbleMgr->rumbleEnabled); i++) {
        rumbleMgr->rumbleEnabled[i] = false;
    }

    if (!rumbleMgr->updateEnabled) {
        // Rumbling update is disabled

        if (D_801D1E70) {
            for (i = 0; i < MAXCONTROLLERS; i++) {
                PadMgr_RumbleSetSingle(i, false);
            }
        }

        D_801D1E70 = rumbleMgr->updateEnabled;
        PadMgr_RumblePause();

        return;
    }

    D_801D1E70 = rumbleMgr->updateEnabled;

    // Start up the manager by wiping old requests
    if (rumbleMgr->state == RUMBLEMANAGER_STATE_INITIAL) {
        for (i = 0; i < MAXCONTROLLERS; i++) {
            PadMgr_RumbleSetSingle(i, false);
        }

        for (i = 0; i < RUMBLE_REQUEST_BUFFER_SIZE; i++) {
            rumbleMgr->requestIntensities[i] = 0;
            rumbleMgr->requestDecayTimers[i] = 0;
            rumbleMgr->requestDecaySteps[i] = 0;
            rumbleMgr->requestAccumulators[i] = 0;
        }

        rumbleMgr->rumblingDuration = 0;
        rumbleMgr->downTime = 0;

        rumbleMgr->overrideIntensity = 0;
        rumbleMgr->overrideDecayTimer = 0;
        rumbleMgr->overrideDecayStep = 0;
        rumbleMgr->overrideAccumulator = 0;

        rumbleMgr->state = RUMBLEMANAGER_STATE_RUNNING;

        PadMgr_RumblePause();
    }

    if (rumbleMgr->state != RUMBLEMANAGER_STATE_WIPE) {
        // Process arrays of rumble requests
        for (i = 0; i < RUMBLE_REQUEST_BUFFER_SIZE; i++) {
            if (rumbleMgr->requestIntensities[i] != 0) { // This entry has a non-empty rumble request
                if (rumbleMgr->requestDecayTimers[i] > 0) {
                    rumbleMgr->requestDecayTimers[i]--;
                } else {
                    temp = rumbleMgr->requestIntensities[i] - rumbleMgr->requestDecaySteps[i];
                    if (temp > 0) {
                        rumbleMgr->requestIntensities[i] = temp;
                    } else {
                        rumbleMgr->requestIntensities[i] = 0;
                    }
                }

                temp = rumbleMgr->requestAccumulators[i] + rumbleMgr->requestIntensities[i];
                rumbleMgr->requestAccumulators[i] = temp; // overflows
                if (strongestIndex == -1) {
                    strongestIndex = i;
                    rumbleMgr->rumbleEnabled[0] = (temp >= 0x100);
                } else if (rumbleMgr->requestIntensities[strongestIndex] < rumbleMgr->requestIntensities[i]) {
                    strongestIndex = i;
                    rumbleMgr->rumbleEnabled[0] = (temp >= 0x100);
                }
            }
        }

        // Process Override request. Note it takes priority over the values set by the request arrays
        if (rumbleMgr->overrideIntensity != 0) {
            if (rumbleMgr->overrideDecayTimer > 0) {
                rumbleMgr->overrideDecayTimer--;
            } else {
                temp = rumbleMgr->overrideIntensity - rumbleMgr->overrideDecayStep;
                if (temp > 0) {
                    rumbleMgr->overrideIntensity = temp;
                } else {
                    rumbleMgr->overrideIntensity = 0;
                }
            }

            temp = rumbleMgr->overrideAccumulator + rumbleMgr->overrideIntensity;
            rumbleMgr->overrideAccumulator = temp; // overflows
            rumbleMgr->rumbleEnabled[0] = (temp >= 0x100);
        }

        if (rumbleMgr->overrideIntensity != 0) {
            temp = rumbleMgr->overrideIntensity;
        } else if (strongestIndex == -1) {
            temp = 0;
        } else {
            temp = rumbleMgr->requestIntensities[strongestIndex];
        }

        // Keep track of how long this have been rumbling (almost) nonstop
        if (temp == 0) {
            rumbleMgr->downTime++;
            if (rumbleMgr->downTime > 5) {
                rumbleMgr->rumblingDuration = 0;
                rumbleMgr->downTime = 5;
            }
        } else {
            rumbleMgr->downTime = 0;
            rumbleMgr->rumblingDuration++;
            if (rumbleMgr->rumblingDuration > 2 * 60 * 60) { // 2 minutes
                // Rumbling has lasted too long, clear system
                rumbleMgr->state = RUMBLEMANAGER_STATE_WIPE;
            }
        }
    } else { // RUMBLEMANAGER_STATE_WIPE
        for (i = 0; i < RUMBLE_REQUEST_BUFFER_SIZE; i++) {
            rumbleMgr->requestIntensities[i] = 0;
            rumbleMgr->requestDecayTimers[i] = 0;
            rumbleMgr->requestDecaySteps[i] = 0;
            rumbleMgr->requestAccumulators[i] = 0;
        }

        rumbleMgr->rumblingDuration = 0;
        rumbleMgr->downTime = 0;

        rumbleMgr->overrideIntensity = 0;
        rumbleMgr->overrideDecayTimer = 0;
        rumbleMgr->overrideDecayStep = 0;
        rumbleMgr->overrideAccumulator = 0;

        PadMgr_RumblePause();
    }
}

void RumbleManager_Init(RumbleManager* rumbleMgr) {
    memset(rumbleMgr, 0, sizeof(RumbleManager));
    rumbleMgr->state = RUMBLEMANAGER_STATE_INITIAL;
    rumbleMgr->updateEnabled = true;
}

void RumbleManager_Destroy(RumbleManager* rumbleMgr) {
}

/**
 * @file sys_rumble.c
 *
 * This file implements a manager for storing and processing rumble pak requests made by the game state. Despite some
 * parts of the system appearing to accommodate all four controller ports, only controller 1 will rumble according to
 * the processed requests.
 * This file is half of the system that runs on the padmgr thread alongside controller communications. The rest of the
 * system that receives the requests from the game state runs on the graph thread and is implemented in `z_rumble.c`.
 *
 * @see RumbleMgr
 * @see z_rumble.c
 *
 * @note Original filename is likely sys_vibrate.c or similar as it is ordered after sys_ucode.c
 */
#include "rumble.h"
#include "padmgr.h"

/**
 * Rumble manager update, runs on Vertical Retrace on the padmgr thread.
 */
void RumbleMgr_Update(RumbleMgr* rumbleMgr) {
    static u8 sWasEnabled = true;
    s32 i;
    s32 strength;
    s32 strongestIndex = -1;

    // Clear enable status for all controllers
    for (i = 0; i < MAXCONTROLLERS; i++) {
        rumbleMgr->rumbleEnable[i] = false;
    }

    if (!rumbleMgr->updateEnabled) {
        if (sWasEnabled) {
            // If it was previously enabled, reset pak type
            for (i = 0; i < MAXCONTROLLERS; i++) {
                gPadMgr.pakType[i] = CONT_PAK_NONE;
            }
        }
        sWasEnabled = rumbleMgr->updateEnabled;
        return;
    }

    sWasEnabled = rumbleMgr->updateEnabled;

    if (rumbleMgr->state == RUMBLE_STATE_RESET) {
        // Reset
        for (i = 0; i < MAXCONTROLLERS; i++) {
            gPadMgr.pakType[i] = CONT_PAK_NONE;
        }

        for (i = 0; i < RUMBLE_MAX_REQUESTS; i++) {
            rumbleMgr->reqAccumulators[i] = 0;
            rumbleMgr->reqDecreaseRates[i] = 0;
            rumbleMgr->reqDurations[i] = 0;
            rumbleMgr->reqStrengths[i] = 0;
        }

        rumbleMgr->onTimer = rumbleMgr->offTimer = rumbleMgr->overrideStrength = rumbleMgr->overrideDuration =
            rumbleMgr->overrideDecreaseRate = rumbleMgr->overrideAccumulator = 0;

        rumbleMgr->state = RUMBLE_STATE_RUNNING;
    }

    if (rumbleMgr->state != RUMBLE_STATE_CLEAR) {
        // Search for index with largest strength
        for (i = 0; i < RUMBLE_MAX_REQUESTS; i++) {
            if (rumbleMgr->reqStrengths[i] != 0) {
                // Non-empty request slot
                if (rumbleMgr->reqDurations[i] > 0) {
                    rumbleMgr->reqDurations[i]--;
                } else {
                    // After duration, decrease the strength by the decrease rate
                    strength = rumbleMgr->reqStrengths[i] - rumbleMgr->reqDecreaseRates[i];
                    rumbleMgr->reqStrengths[i] = MAX(strength, 0);
                }

                // Increment accumulator by the strength
                strength = rumbleMgr->reqAccumulators[i] + rumbleMgr->reqStrengths[i];
                rumbleMgr->reqAccumulators[i] = strength;

                if (strongestIndex == -1) {
                    strongestIndex = i;
                    // Rumble is enabled on the controller only when there is overflow of the accumulator, overflow
                    // will happen more often for larger request strengths making it feel stronger to the player
                    rumbleMgr->rumbleEnable[0] = strength > 255;
                } else if (rumbleMgr->reqStrengths[i] > rumbleMgr->reqStrengths[strongestIndex]) {
                    strongestIndex = i;
                    rumbleMgr->rumbleEnable[0] = strength > 255;
                }
            }
        }

        if (rumbleMgr->overrideStrength != 0) {
            // Set override
            if (rumbleMgr->overrideDuration > 0) {
                rumbleMgr->overrideDuration--;
            } else {
                // Once the duration is over, start decrementing the strength
                strength = rumbleMgr->overrideStrength - rumbleMgr->overrideDecreaseRate;
                rumbleMgr->overrideStrength = MAX(strength, 0);
            }
            // Increment accumulator, set rumble enabled on overflow
            strength = rumbleMgr->overrideAccumulator + rumbleMgr->overrideStrength;
            rumbleMgr->overrideAccumulator = strength;
            rumbleMgr->rumbleEnable[0] = strength > 255;
        }

        if (rumbleMgr->overrideStrength != 0) {
            strength = rumbleMgr->overrideStrength;
        } else {
            strength = (strongestIndex == -1) ? 0 : rumbleMgr->reqStrengths[strongestIndex];
        }

        if (strength == 0) {
            // No rumble
            if ((++rumbleMgr->offTimer) > 5) {
                // After 5 VIs with no rumble, reset the rumble on timer
                rumbleMgr->onTimer = 0;
                rumbleMgr->offTimer = 5;
            }
        } else {
            // Rumble
            rumbleMgr->offTimer = 0;
            if ((++rumbleMgr->onTimer) > 7200) { // 2 minutes at 60 VI/s, 2 minutes 24 seconds at 50 VI/s
                // Clear all requests if rumble has been on for too long
                rumbleMgr->state = RUMBLE_STATE_CLEAR;
            }
        }
    } else {
        // Clear all requests
        for (i = 0; i < RUMBLE_MAX_REQUESTS; i++) {
            rumbleMgr->reqAccumulators[i] = 0;
            rumbleMgr->reqDecreaseRates[i] = 0;
            rumbleMgr->reqDurations[i] = 0;
            rumbleMgr->reqStrengths[i] = 0;
        }

        // Clear override request
        rumbleMgr->onTimer = rumbleMgr->offTimer = rumbleMgr->overrideStrength = rumbleMgr->overrideDuration =
            rumbleMgr->overrideDecreaseRate = rumbleMgr->overrideAccumulator = 0;
    }
}

void RumbleMgr_Init(RumbleMgr* rumbleMgr) {
    bzero(rumbleMgr, sizeof(RumbleMgr));
    rumbleMgr->state = RUMBLE_STATE_RESET;
    rumbleMgr->updateEnabled = true;
}

void RumbleMgr_Destroy(RumbleMgr* rumbleMgr) {
#if DEBUG_FEATURES
    bzero(rumbleMgr, sizeof(RumbleMgr));
#endif
}

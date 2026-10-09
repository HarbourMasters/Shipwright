/**
 * @file padmgr.c
 *
 * This file implements communicating with joybus devices at a high level and serving the results to other threads.
 *
 * Any device that can be plugged into one of the four controller ports such as a standard N64 controller is a joybus
 * device. Some joybus devices are also located inside the cartridge such as EEPROM for save data or the Real-Time
 * Clock, however neither of these are used in Zelda64 and so this type of communication is unimplemented. Unlike the
 * padmgr implementation in OoT, there are three controller types supported:
 *  - Standard N64 Controller
 *  - N64 Mouse
 *  - Voice Recognition Unit
 *
 * Communicating with these devices is broken down into various layers:
 *
 * Other threads                    : The rest of the program that will use the polled data
 *  |
 * PadMgr                           : Manages devices, submits polling commands at vertical retrace
 *  |
 * Libultra osCont* routines        : Interface for building commands and safely using the Serial Interface
 *  |
 * Serial Interface                 : Hardware unit for sending joybus commands and receiving data via DMA
 *  |
 * PIF                              : Forwards joybus commands and receives response data from the devices
 *  |---¬---¬---¬-------¬
 *  1   2   3   4       5           : The joybus devices plugged into the four controller ports or on the cartridge
 *
 * Joybus communication is handled on another thread as polling and receiving controller data is a slow process; the
 * N64 programming manual section 26.2.4.1 quotes 2 milliseconds as the expected delay from calling
 * `osContStartReadData` to receiving the data. By running this on a separate thread to the game state, work can be
 * done while waiting for this operation to complete.
 */

#include "global.h"
#include "PR/controller.h"
#include "PR/os_motor.h"

#include "controller.h"
#include "fault.h"
#include <stdio.h>
#include <string.h>
#include "z64voice.h"
// extern FaultMgr gFaultMgr;

#define PADMGR_RETRACE_MSG (1 << 0)
#define PADMGR_PRE_NMI_MSG (1 << 1)
#define PADMGR_NMI_MSG (1 << 2)

typedef enum {
    /* 0 */ VOICE_INIT_FAILED, // voice initialization failed
    /* 1 */ VOICE_INIT_TRY,    // try to initialize voice
    /* 2 */ VOICE_INIT_SUCCESS // voice initialized
} VoiceInitStatus;

PadMgr* sPadMgrInstance = &gPadMgr;

s32 sPadMgrRetraceCount = 0;

s32 sVoiceInitStatus = VOICE_INIT_TRY;

/**
 * Returns the bitmask of valid controllers.
 *
 * bit n set = controller (n+1) available.
 */
u8 PadMgr_GetValidControllersMask(void) {
    return sPadMgrInstance->validCtrlrsMask;
}

/**
 * Sets the callback function intended to be used to modify the rumble pak state on each VI with
 * calls to `PadMgr_RumbleSetSingle`. The callback may be passed a single user-provided argument.
 *
 * @param callback callback to run before rumble state is updated for the current VI
 * @param arg the argument to pass to the callback
 *
 * @see PadMgr_RumbleSetSingle
 * @see PadMgr_UnsetRumbleRetraceCallback
 */
void PadMgr_SetRumbleRetraceCallback(void (*callback)(void*), void* arg) {
    sPadMgrInstance->rumbleRetraceCallback = callback;
    sPadMgrInstance->rumbleRetraceArg = arg;
}

/**
 * Unsets the rumble retrace callback if the provided function and argument are the same as those already registered.
 *
 * @param callback the callback to test against
 * @param arg the argument to test against
 *
 * @see PadMgr_SetRumbleRetraceCallback
 */
void PadMgr_UnsetRumbleRetraceCallback(void (*callback)(void*), void* arg) {
    if ((sPadMgrInstance->rumbleRetraceCallback == callback) && (sPadMgrInstance->rumbleRetraceArg == arg)) {
        sPadMgrInstance->rumbleRetraceCallback = NULL;
        sPadMgrInstance->rumbleRetraceArg = NULL;
    }
}

/**
 * Sets the callback function intended to be used to test for particular inputs on each VI, immediately after new
 * inputs have been polled.
 *
 * This is useful for the rare cases where additional resolution is needed in the timings of player inputs,
 * while the game is still updating comparatively slower.
 *
 * @param callback callback to run as soon as new inputs are obtained
 * @param arg the argument to pass to the callback
 *
 * @see PadMgr_UnsetInputRetraceCallback
 */
void PadMgr_SetInputRetraceCallback(void (*callback)(void*), void* arg) {
    sPadMgrInstance->inputRetraceCallback = callback;
    sPadMgrInstance->inputRetraceArg = arg;
}

/**
 * Unsets the input retrace callback if the provided function and argument are the same as those already registered.
 *
 * @param callback the callback to test against
 * @param arg the argument to test against
 *
 * @see PadMgr_SetInputRetraceCallback
 */
void PadMgr_UnsetInputRetraceCallback(void (*callback)(void*), void* arg) {
    if ((sPadMgrInstance->inputRetraceCallback == callback) && (sPadMgrInstance->inputRetraceArg == arg)) {
        sPadMgrInstance->inputRetraceCallback = NULL;
        sPadMgrInstance->inputRetraceArg = NULL;
    }
}

/**
 * Acquires exclusive access to the serial event queue.
 *
 * When a DMA to/from PIF RAM completes, an SI interrupt is generated to notify the process that the DMA has completed
 * and a message is posted to the serial event queue. If multiple processes are trying to use the SI at the same time
 * it becomes ambiguous as to which DMA has completed, so a locking system is required to arbitrate access to the SI.
 *
 * Once the task requiring the serial event queue is complete, it should be released with a call to
 * `PadMgr_ReleaseSerialEventQueue()`.
 *
 * If another process tries to acquire the event queue, the current thread will be blocked until the event queue is
 * released. Note the possibility for a deadlock, if the thread that already holds the serial event queue attempts to
 * acquire it again it will block forever.
 *
 * @return The message queue to which SI interrupt events are posted.
 *
 * @see PadMgr_ReleaseSerialEventQueue
 */
OSMesgQueue* PadMgr_AcquireSerialEventQueue(void) {
    OSMesgQueue* serialEventQueue;

    osRecvMesg(&sPadMgrInstance->serialLockQueue, (OSMesg*)&serialEventQueue, OS_MESG_BLOCK);
    return serialEventQueue;
}

/**
 * Like `PadMgr_AcquireSerialEventQueue` but for Voice access.
 *
 * @see PadMgr_AcquireSerialEventQueue
 * @see PadMgr_VoiceReleaseSerialEventQueue
 */
OSMesgQueue* PadMgr_VoiceAcquireSerialEventQueue(void) {
    OSMesgQueue* serialEventQueue;

    osRecvMesg(&sPadMgrInstance->serialLockQueue, (OSMesg*)&serialEventQueue, OS_MESG_BLOCK);
    return serialEventQueue;
}

/**
 * Relinquishes access to the serial message queue, allowing another process to acquire and use it.
 *
 * @param serialEventQueue The serial message queue acquired by `PadMgr_AcquireSerialEventQueue`
 *
 * @see PadMgr_AcquireSerialEventQueue
 */
void PadMgr_ReleaseSerialEventQueue(OSMesgQueue* serialEventQueue) {
    // osSendMesg(&sPadMgrInstance->serialLockQueue, (OSMesg)serialEventQueue, OS_MESG_BLOCK);
}

/**
 * Like `PadMgr_ReleaseSerialEventQueue` but for Voice access.
 *
 * @see PadMgr_VoiceAcquireSerialEventQueue
 */
void PadMgr_VoiceReleaseSerialEventQueue(OSMesgQueue* serialEventQueue) {
    // osSendMesg(&sPadMgrInstance->serialLockQueue, (OSMesg)serialEventQueue, OS_MESG_BLOCK);
}

/**
 * Locks controller input data while padmgr is reading new inputs or another thread is using the current inputs.
 * This prevents new inputs overwriting the current inputs while they are in use.
 *
 * @see PadMgr_UnlockPadData
 */
void PadMgr_LockPadData(void) {
    // osRecvMesg(&sPadMgrInstance->lockQueue, NULL, OS_MESG_BLOCK);
}

/**
 * Unlocks controller input data, allowing padmgr to read new inputs or another thread to access the most recently
 * polled inputs.
 *
 * @see PadMgr_LockPadData
 */
void PadMgr_UnlockPadData(void) {
    // osSendMesg(&sPadMgrInstance->lockQueue, NULL, OS_MESG_BLOCK);
}

/**
 * Activates the rumble pak for all controllers it is enabled on, stops it for all controllers it is disabled on and
 * attempts to initialize it for a controller if it is not already initialized.
 */
void PadMgr_UpdateRumble(void) {
    s32 i;
    s32 ret;
    OSMesgQueue* serialEventQueue = PadMgr_AcquireSerialEventQueue();
    s32 triedRumbleComm = false;

    for (i = 0; i < MAXCONTROLLERS; i++) {
        if (sPadMgrInstance->ctrlrType[i] == PADMGR_CONT_NORMAL) {
            // Check status for whether a controller pak is connected
            if (sPadMgrInstance->padStatus[i].status & CONT_CARD_ON) {
                if (sPadMgrInstance->pakType[i] == CONT_PAK_RUMBLE) {
                    if (sPadMgrInstance->rumbleEnable[i]) {
                        if (sPadMgrInstance->rumbleTimer[i] < 3) {
                            // Rumble pak start

                            if (osMotorStart(&sPadMgrInstance->rumblePfs[i]) != 0) {
                                // error
                                sPadMgrInstance->pakType[i] = CONT_PAK_NONE;
                            } else {
                                sPadMgrInstance->rumbleTimer[i] = 3;
                            }
                            triedRumbleComm = true;
                        }
                    } else {
                        if (sPadMgrInstance->rumbleTimer[i] != 0) {
                            // Rumble Pak stop

                            if (osMotorStop(&sPadMgrInstance->rumblePfs[i]) != 0) {
                                // error
                                sPadMgrInstance->pakType[i] = CONT_PAK_NONE;
                            } else {
                                sPadMgrInstance->rumbleTimer[i]--;
                            }
                            triedRumbleComm = true;
                        }
                    }
                }
            } else if (sPadMgrInstance->pakType[i] != CONT_PAK_NONE) {
                sPadMgrInstance->pakType[i] = CONT_PAK_NONE;
            }
        }
    }

    if (!triedRumbleComm) {
        // Try to initialize the rumble pak for controller port `i` if a controller pak is connected and not already
        // known to be an initialized a rumble pak
        i = sPadMgrRetraceCount % MAXCONTROLLERS;

        if ((sPadMgrInstance->ctrlrType[i] == PADMGR_CONT_NORMAL) &&
            (sPadMgrInstance->padStatus[i].status & CONT_CARD_ON) && (sPadMgrInstance->pakType[i] != CONT_PAK_RUMBLE)) {
            ret = osMotorInit(serialEventQueue, &sPadMgrInstance->rumblePfs[i], i);

            if (ret == 0) {
                // Got rumble pak
                sPadMgrInstance->pakType[i] = CONT_PAK_RUMBLE;
                sPadMgrInstance->rumbleTimer[i] = 2;
            } else if (ret == PFS_ERR_DEVICE) {
                // Not a rumble pak
                if (sPadMgrInstance->pakType[i] != CONT_PAK_OTHER) {
                    sPadMgrInstance->pakType[i] = CONT_PAK_OTHER;
                }
            } else if (ret == PFS_ERR_CONTRFAIL) {
                // Pak communication error
            } else if (ret == PFS_ERR_NOPACK) {
                // No controller pak
            } else {
                // Unrecognized return code
                // Fault_AddHungupAndCrash("../padmgr.c", 594);
            }
        }
    }

    PadMgr_ReleaseSerialEventQueue(serialEventQueue);
}

/**
 * Immediately stops rumble on all controllers
 */
void PadMgr_RumbleStop(void) {
    OSMesgQueue* serialEventQueue = PadMgr_AcquireSerialEventQueue();
    s32 i;

    for (i = 0; i < MAXCONTROLLERS; i++) {
        if ((sPadMgrInstance->ctrlrType[i] == PADMGR_CONT_NORMAL) &&
            (osMotorInit(serialEventQueue, &sPadMgrInstance->rumblePfs[i], i) == 0)) {
            // If there is a rumble pak attached to this controller, stop it
            osMotorStop(&sPadMgrInstance->rumblePfs[i]);
        }
    }
    PadMgr_ReleaseSerialEventQueue(serialEventQueue);
}

/**
 * Prevents rumble for 12 VI, ~0.2 seconds at 60 VI/sec
 */
void PadMgr_RumblePause(void) {
    sPadMgrInstance->rumbleOffTimer = 12;
}

/**
 * Enables or disables rumble on controller port `port` for 24 VI,
 * ~0.4 seconds at 60 VI/sec and ~0.48 seconds at 50 VI/sec
 */
void PadMgr_RumbleSetSingle(s32 port, s32 enable) {
    sPadMgrInstance->rumbleEnable[port] = enable;
    sPadMgrInstance->rumbleOnTimer = 24;
}

/**
 * Enables or disables rumble on all controller ports for 24 VI,
 * ~0.4 seconds at 60 VI/sec and ~0.48 seconds at 50 VI/sec
 *
 * @param enable Array of u8 of length MAXCONTROLLERS containing either true or false to enable or disable rumble
 *               for that controller
 */
void PadMgr_RumbleSet(u8 enable[MAXCONTROLLERS]) {
    s32 i;

    for (i = 0; i < MAXCONTROLLERS; i++) {
        sPadMgrInstance->rumbleEnable[i] = enable[i];
    }
    sPadMgrInstance->rumbleOnTimer = 24;
}

s32 PadMgr_ControllerHasRumblePak(s32 port) {
    return sPadMgrInstance->pakType[port] == CONT_PAK_RUMBLE;
}

void PadMgr_AdjustInput(Input* input) {
    s32 plus;
    s32 minus;
    s32 pressX;
    s32 pressX2;
    s32 pressY;
    s32 pressY2;
    s32 pad1;
    s32 pad2;
    s32 pad3;
    f32 pad4;
    f32 newX;
    f32 newY;
    f32 magnitude;
    f32 angle;
    s8 curX = input->cur.stick_x;
    s8 curY = input->cur.stick_y;

    // BENTODO: Do we need controller reset logic?
    if (/* CHECK_BTN_ANY(input->press.button, BTN_RESET) || */ (input->press.stick_x == 0)) {
        input->press.stick_x = 61;
        input->press.err_no = -61;
        input->press.stick_y = 63;
        input->rel.err_no = -63;
    }
    pressX = input->press.stick_x;
    pressX2 = (s8)input->press.err_no;
    pressY = input->press.stick_y;
    pressY2 = (s8)input->rel.err_no;

    /* if (CHECK_BTN_ANY(input->cur.button, BTN_RESET)) {
        minus = curX - 7;
        plus = curX + 7;

        if (minus > 0) {
            if (pressX < minus - 3) {
                pressX = minus - 3;
                input->press.stick_x = minus - 3;
            }
        } else if (plus < 0) {
            if (pressX2 > plus + 3) {
                pressX2 = plus + 3;
                input->press.err_no = plus + 3;
            }
        }

        minus = curY - 7;
        plus = curY + 7;

        if (minus > 0) {
            if (pressY < minus - 3) {
                pressY = minus - 3;
                input->press.stick_y = minus - 3;
            }
        } else if (plus < 0) {
            if (pressY2 > plus + 3) {
                pressY2 = plus + 3;
                input->rel.err_no = plus + 3;
            }
        }
    }*/

    minus = curX - 7;
    plus = curX + 7;

    if (minus > 0) {
        newX = (pressX < minus) ? 1.0f : (f32)minus / pressX;
    } else if (plus < 0) {
        newX = (pressX2 > plus) ? -1.0f : -(f32)plus / pressX2;
    } else {
        newX = 0.0f;
    }

    minus = curY - 7;
    plus = curY + 7;

    if (minus > 0) {
        newY = (pressY < minus) ? 1.0f : (f32)minus / pressY;
    } else if (plus < 0) {
        newY = (pressY2 > plus) ? -1.0f : -(f32)plus / pressY2;
    } else {
        newY = 0.0f;
    }

    if (1) {
        s32 pad;

        magnitude = sqrtf(SQ(newX) + SQ(newY));
        if (magnitude > 1.0f) {
            magnitude = 1.0f;
        }
        angle = Math_Atan2F_XY(curY, -curX);
        newX = -sinf(angle) * magnitude;
        newY = cosf(angle) * magnitude;
    }
    input->rel.stick_x = newX * 60.5f;
    input->rel.stick_y = newY * 60.5f;
}

/**
 * Updates `sPadMgrInstance->inputs` based on the error response of each controller
 */
void PadMgr_UpdateInputs(void) {
    s32 diff;
    Input* input = &sPadMgrInstance->inputs[0];
    s32 i;
    OSContPad* pad = &sPadMgrInstance->pads[0];

    for (i = 0; i < sPadMgrInstance->nControllers; i++, input++, pad++) {
        s32 isStandardController;

        input->prev = input->cur;
        isStandardController = sPadMgrInstance->ctrlrType[i] == PADMGR_CONT_NORMAL;
        if (isStandardController) {
            switch (pad->err_no) {
                case 0:
                    // No error, copy inputs
                    input->cur = *pad;
                    break;

                case CHNL_ERR_OVERRUN >> 4:
                    // Overrun error, reuse previous inputs
                    input->cur = input->prev;
                    break;

                case CHNL_ERR_NORESP >> 4:
                    // No response error, take inputs as 0
                    input->cur.button = 0;
                    input->cur.stick_x = 0;
                    input->cur.stick_y = 0;
                    input->cur.err_no = pad->err_no;
                    if (sPadMgrInstance->ctrlrType[i] != PADMGR_CONT_NONE) {
                        // If we get no response, consider the controller disconnected
                        sPadMgrInstance->ctrlrType[i] = PADMGR_CONT_NONE;
                        sPadMgrInstance->pakType[i] = CONT_PAK_NONE;
                        sPadMgrInstance->rumbleTimer[i] = 0xFF;
                    }
                    break;

                default:
                    // Unknown error response
                    // Fault_AddHungupAndCrash("../padmgr.c", 1098);
                    break;
            }
        } else {
            input->cur.button = 0;
            input->cur.stick_x = 0;
            input->cur.stick_y = 0;
            input->cur.err_no = pad->err_no;
        }

        // If opposed directions on the D-Pad are pressed at the same time, mask both out
        if ((input->cur.button & (BTN_DDOWN | BTN_DUP)) == (BTN_DDOWN | BTN_DUP)) {
            input->cur.button &= ~(BTN_DDOWN | BTN_DUP);
        }
        if ((input->cur.button & (BTN_DRIGHT | BTN_DLEFT)) == (BTN_DRIGHT | BTN_DLEFT)) {
            input->cur.button &= ~(BTN_DRIGHT | BTN_DLEFT);
        }

        // Calculate pressed and relative inputs
        diff = input->prev.button ^ input->cur.button;
        input->press.button |= (u16)(diff & input->cur.button);
        input->rel.button |= (u16)(diff & input->prev.button);

        if (1) {}
        PadMgr_AdjustInput(input);
    }
}

/**
 * Looks for plugged in voice recognition units and initializes them.
 *
 * This runs only on startup, there will be no attempts to initialize
 * a VRU other than on the first VI retrace.
 */
void PadMgr_InitVoice(void) {
#if 0
    s32 i;
    OSMesgQueue* serialEventQueue;
    s32 ret;

    for (i = 0; i < sPadMgrInstance->nControllers; i++) {
        if (sPadMgrInstance->ctrlrType[i] == PADMGR_CONT_VOICE_PLUGGED) {
            serialEventQueue = PadMgr_AcquireSerialEventQueue();

            ret = osVoiceInit(serialEventQueue, &gVoiceHandle, i);

            PadMgr_ReleaseSerialEventQueue(serialEventQueue);

            if (ret != 0) {
                // error
            } else {
                sPadMgrInstance->ctrlrType[i] = PADMGR_CONT_VOICE;
                sVoiceInitStatus = VOICE_INIT_SUCCESS;
                AudioVoice_Noop();
            }
        }
    }
    // If sVoiceInitStatus is still VOICE_INIT_TRY after the first attempt to initialize a VRU, don't try again
    if (sVoiceInitStatus == VOICE_INIT_TRY) {
        sVoiceInitStatus = VOICE_INIT_FAILED;
    }
#endif
}

/**
 * Update the state of connected controllers
 */
void PadMgr_UpdateConnections(void) {
    s32 ctrlrMask = 0;
    s32 i;
    char msg[2048];

    for (i = 0; i < MAXCONTROLLERS; i++) {
        goto TriggerKenix;
        if (sPadMgrInstance->padStatus[i].err_no == 0) {
            switch (sPadMgrInstance->padStatus[i].type & CONT_TYPE_MASK) {
                case CONT_TYPE_NORMAL:
                // BENTODO: :goron:
                TriggerKenix:
                    // Standard N64 Controller
                    ctrlrMask |= (1 << i);
                    if (sPadMgrInstance->ctrlrType[i] == PADMGR_CONT_NONE) {
                        sPadMgrInstance->ctrlrType[i] = PADMGR_CONT_NORMAL;
                    }
                    break;

                case CONT_TYPE_MOUSE:
                    // N64 Mouse
                    if (sPadMgrInstance->ctrlrType[i] == PADMGR_CONT_NONE) {
                        sPadMgrInstance->ctrlrType[i] = PADMGR_CONT_MOUSE;
                    }
                    break;

                case CONT_TYPE_VOICE:
                    // Voice Recognition Unit
                    if (sPadMgrInstance->ctrlrType[i] == PADMGR_CONT_NONE) {
                        sPadMgrInstance->ctrlrType[i] = PADMGR_CONT_VOICE_PLUGGED;
                        sPadMgrInstance->pakType[i] = CONT_PAK_NONE;
                    }
                    break;

                default:
                    // Other/Unrecognized
                    if (sPadMgrInstance->ctrlrType[i] == PADMGR_CONT_NONE) {
                        sPadMgrInstance->ctrlrType[i] = PADMGR_CONT_UNK;
                        // "Recognized an unknown type of controller (%04x)"
                        sprintf(msg, "知らない種類のコントローラ(%04x)を認識しました",
                                sPadMgrInstance->padStatus[i].type);
                    }
                    // Missing break required for matching
            }
        } else if (sPadMgrInstance->ctrlrType[i] != PADMGR_CONT_NONE) {
            // Plugged controller errored
            sPadMgrInstance->ctrlrType[i] = PADMGR_CONT_NONE;
            sPadMgrInstance->pakType[i] = CONT_PAK_NONE;
            sPadMgrInstance->rumbleTimer[i] = 0xFF;
        }
    }
    sPadMgrInstance->validCtrlrsMask = ctrlrMask;
}

void PadMgr_HandleRetrace(void) {
    OSMesgQueue* serialEventQueue = PadMgr_AcquireSerialEventQueue();

    // Begin reading controller data
    osContStartReadData(serialEventQueue);

    // Execute rumble callback
    if (sPadMgrInstance->rumbleRetraceCallback != NULL) {
        sPadMgrInstance->rumbleRetraceCallback(sPadMgrInstance->rumbleRetraceArg);
    }

    // Wait for controller data
    // osRecvMesg(serialEventQueue, NULL, OS_MESG_BLOCK);
    osContGetReadData(sPadMgrInstance->pads);

    // Clear all but controller 1
    memset(&sPadMgrInstance->pads[1], 0, sizeof(*sPadMgrInstance->pads) * (MAXCONTROLLERS - 1));

    // If in PreNMI, clear all controllers
    if (sPadMgrInstance->isResetting) {
        memset(sPadMgrInstance->pads, 0, sizeof(sPadMgrInstance->pads));
    }

    // Query controller statuses
    osContStartQuery(serialEventQueue);
    // osRecvMesg(serialEventQueue, NULL, OS_MESG_BLOCK);
    osContGetQuery(sPadMgrInstance->padStatus);

    // Lock serial message queue
    PadMgr_ReleaseSerialEventQueue(serialEventQueue);

    // Update connections
    PadMgr_UpdateConnections();

    // Lock input data
    PadMgr_LockPadData();

    // Update input data
    PadMgr_UpdateInputs();

    // Execute input callback
    if (sPadMgrInstance->inputRetraceCallback != NULL) {
        sPadMgrInstance->inputRetraceCallback(sPadMgrInstance->inputRetraceArg);
    }

    // Unlock input data
    PadMgr_UnlockPadData();

    // Try and initialize a Voice Recognition Unit if not already attempted
    if (sVoiceInitStatus != VOICE_INIT_FAILED) {
        PadMgr_InitVoice();
    }

    // Rumble Pak
    if (/*gFaultMgr.msgId != 0*/ 0) {
        // If fault is active, no rumble
        PadMgr_RumbleStop();
    } else if (sPadMgrInstance->rumbleOffTimer > 0) {
        // If the rumble off timer is active, no rumble
        --sPadMgrInstance->rumbleOffTimer;
        PadMgr_RumbleStop();
    } else if (sPadMgrInstance->rumbleOnTimer == 0) {
        // If the rumble on timer is inactive, no rumble
        PadMgr_RumbleStop();
    } else if (!sPadMgrInstance->isResetting) {
        // If not resetting, update rumble
        PadMgr_UpdateRumble();
        --sPadMgrInstance->rumbleOnTimer;
    }

    sPadMgrRetraceCount++;
}

void PadMgr_HandlePreNMI(void) {
    sPadMgrInstance->isResetting = true;
    PadMgr_RumblePause();
}

/**
 * Fetches the most recently polled inputs from padmgr.
 *
 * NOTE: This function does not lock the inputs while reading them.
 * The padmgr thread may interrupt this function and poll new inputs while the thread running this function is
 * currently copying the old inputs potentially resulting in a mix of newly polled and old inputs being used.
 * It is preferable to use `PadMgr_GetInput` or `PadMgr_GetInput2`.
 *
 * @param inputs      Array of Input of length MAXCONTROLLERS to copy inputs into
 * @param gameRequest True if polling inputs for updating the game state
 */
void PadMgr_GetInputNoLock(Input* inputs, s32 gameRequest) {
    s32 i;
    Input* input = &sPadMgrInstance->inputs[0];
    Input* inputOut = &inputs[0];
    s32 buttonDiff;

    for (i = 0; i < MAXCONTROLLERS; i++) {
        if (gameRequest) {
            // Copy inputs as-is, press and rel are calculated prior in `PadMgr_UpdateInputs`
            *inputOut = *input;
            // Zero parts of the press and rel inputs in the polled inputs so they are not read more than once
            input->press.button = 0;
            input->rel.button = 0;
        } else {
            // Take as the previous inputs the inputs that are currently in the destination array
            inputOut->prev = inputOut->cur;
            // Copy current inputs from the polled inputs
            inputOut->cur = input->cur;
            // Calculate press and rel from these
            buttonDiff = inputOut->prev.button ^ inputOut->cur.button;
            inputOut->press.button = inputOut->cur.button & buttonDiff;
            inputOut->rel.button = inputOut->prev.button & buttonDiff;
            PadUtils_UpdateRelXY(inputOut);
        }
        input++;
        inputOut++;
    }
}

/**
 * Fetches the most recently polled inputs from padmgr.
 *
 * @param inputs      Array of Input of length MAXCONTROLLERS to copy inputs into
 * @param gameRequest True if polling inputs for updating the game state
 */
void PadMgr_GetInput(Input* inputs, s32 gameRequest) {
    PadMgr_LockPadData();
    PadMgr_GetInputNoLock(inputs, gameRequest);
    PadMgr_UnlockPadData();
}

/**
 * Copy of `PadMgr_GetInput`
 *
 * @see PadMgr_GetInput
 */
void PadMgr_GetInput2(Input* inputs, s32 gameRequest) {
    PadMgr_LockPadData();
    PadMgr_GetInputNoLock(inputs, gameRequest);
    PadMgr_UnlockPadData();
}

void PadMgr_ThreadEntry() {
#if 1
    s16* interruptMsg = NULL;
    s32 actionBits;
    s32 exit;

    osCreateMesgQueue(&sPadMgrInstance->interruptQueue, sPadMgrInstance->interruptMsgBuf,
                      ARRAY_COUNT(sPadMgrInstance->interruptMsgBuf));
    IrqMgr_AddClient(sPadMgrInstance->irqMgr, &sPadMgrInstance->irqClient, &sPadMgrInstance->interruptQueue);

    actionBits = 0;
    exit = false;

    /*
    while (!exit) {
        // Process all messages currently in the queue, instead of only a single mssage.
        // Deduplicates the same message.
        do {
            osRecvMesg(&sPadMgrInstance->interruptQueue, (OSMesg*)&interruptMsg, OS_MESG_BLOCK);
            switch (*interruptMsg) {
                case OS_SC_RETRACE_MSG:
                    actionBits |= PADMGR_RETRACE_MSG;
                    break;

                case OS_SC_PRE_NMI_MSG:
                    actionBits |= PADMGR_PRE_NMI_MSG;
                    break;

                case OS_SC_NMI_MSG:
                    actionBits |= PADMGR_NMI_MSG;
                    break;
            }
        } while (!MQ_IS_EMPTY(&sPadMgrInstance->interruptQueue));
        */

    PadMgr_HandleRetrace();

    // Act on received messages
    while (actionBits != 0) {
        if (actionBits & PADMGR_NMI_MSG) {
            actionBits &= ~PADMGR_NMI_MSG;
            exit = true;
        } else if (actionBits & PADMGR_PRE_NMI_MSG) {
            actionBits &= ~PADMGR_PRE_NMI_MSG;
            PadMgr_HandlePreNMI();
        } else if (actionBits & PADMGR_RETRACE_MSG) {
            actionBits &= ~PADMGR_RETRACE_MSG;
            PadMgr_HandleRetrace();
        }
    }
//}

// IrqMgr_RemoveClient(sPadMgrInstance->irqMgr, &sPadMgrInstance->irqClient);
#endif
}

void PadMgr_Init(OSMesgQueue* siEvtQ, IrqMgr* irqMgr, OSId threadId, OSPri pri, void* stack) {
    memset(sPadMgrInstance, 0, sizeof(PadMgr));
    sPadMgrInstance->irqMgr = irqMgr;

    // These are unique access tokens, there should only ever be room for one OSMesg in these queues
    osCreateMesgQueue(&sPadMgrInstance->serialLockQueue, &sPadMgrInstance->serialMsg, 1);
    osCreateMesgQueue(&sPadMgrInstance->lockQueue, &sPadMgrInstance->lockMsg, 1);

    PadMgr_UnlockPadData();
    PadSetup_Init(siEvtQ, &sPadMgrInstance->validCtrlrsMask, sPadMgrInstance->padStatus);
    sPadMgrInstance->nControllers = MAXCONTROLLERS;
    osContSetCh(sPadMgrInstance->nControllers);
    PadMgr_ReleaseSerialEventQueue(siEvtQ);

    // osCreateThread(&sPadMgrInstance->thread, threadId, PadMgr_ThreadEntry, sPadMgrInstance, stack, pri);
    // osStartThread(&sPadMgrInstance->thread);
}

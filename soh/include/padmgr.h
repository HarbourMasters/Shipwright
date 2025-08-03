#ifndef PADMGR_H
#define PADMGR_H

#include "ultra64.h"
#include "libu64/pad.h"
#include "irqmgr.h"

typedef enum ControllerPakType {
    CONT_PAK_NONE,
    CONT_PAK_RUMBLE,
    CONT_PAK_OTHER
} ControllerPakType;

typedef struct PadMgr {
    /* 0x0000 */ OSContStatus padStatus[MAXCONTROLLERS];
    /* 0x0010 */ OSMesg serialMsg;
    /* 0x0014 */ OSMesg lockMsg;
    /* 0x0018 */ OSMesg interruptMsgBuf[4];
    /* 0x0028 */ OSMesgQueue serialLockQueue;
    /* 0x0040 */ OSMesgQueue lockQueue;
    /* 0x0058 */ OSMesgQueue interruptQueue;
    /* 0x0070 */ IrqMgrClient irqClient;
    /* 0x0078 */ IrqMgr* irqMgr;
    /* 0x0080 */ OSThread thread;
    /* 0x0230 */ Input inputs[MAXCONTROLLERS];
    /* 0x0290 */ OSContPad pads[MAXCONTROLLERS];
    /* 0x02A8 */ vu8 validCtrlrsMask;
    /* 0x02A9 */ u8 nControllers;
    /* 0x02AA */ u8 ctrlrIsConnected[MAXCONTROLLERS];
    /* 0x02AE */ u8 pakType[MAXCONTROLLERS];
    /* 0x02B2 */ vu8 rumbleEnable[MAXCONTROLLERS];
    /* 0x02B6 */ u8 rumbleTimer[MAXCONTROLLERS];
    /* 0x02BC */ OSPfs rumblePfs[MAXCONTROLLERS];
    /* 0x045C */ vu8 rumbleOffTimer; // amount of VI retraces to not rumble for, takes priority over rumbleOnTimer
    /* 0x045D */ vu8 rumbleOnTimer; // amount of VI retraces to rumble for
    /* 0x045E */ u8 isResetting;
    /* 0x0460 */ void (*retraceCallback)(struct PadMgr* padMgr, void* arg);
    /* 0x0464 */ void* retraceCallbackArg;
} PadMgr; // size = 0x468

// Initialization

void PadMgr_Init(PadMgr* padMgr, OSMesgQueue* serialEventQueue, IrqMgr* irqMgr, OSId id, OSPri priority, void* stack);

// Fetching inputs

// This function cannot be prototyped here in all configurations because it is called incorrectly in fault_gc.c
// (see bug in `Fault_PadCallback`)
#if PLATFORM_N64 || defined(AVOID_UB)
void PadMgr_RequestPadData(PadMgr* padmgr, Input* inputs, s32 gameRequest);
#endif

// For internal use by Controller Pak systems

OSMesgQueue* PadMgr_AcquireSerialEventQueue(PadMgr* padMgr);
void PadMgr_ReleaseSerialEventQueue(PadMgr* padMgr, OSMesgQueue* serialEventQueue);

// Rumble

void PadMgr_RumbleStop(PadMgr* padMgr);
void PadMgr_RumbleReset(PadMgr* padMgr);
void PadMgr_RumbleSetSingle(PadMgr* padMgr, u32 port, u32 rumble);
void PadMgr_RumbleSet(PadMgr* padMgr, u8* enable);

// Retrace callback

/**
 * Sets the padmgr retrace callback that runs while waiting for controller input. The callback may be passed a single
 * user-provided argument. The callback function should be `void (*)(PadMgr*, void*)`.
 *
 * @param callback callback to run before rumble state is updated for the current VI
 * @param arg the argument to pass to the callback
 *
 * @see PADMGR_UNSET_RETRACE_CALLACK
 */
#define PADMGR_SET_RETRACE_CALLACK(padmgr, callback, arg) \
    do {                                                  \
        (padmgr)->retraceCallback = (callback);           \
        (padmgr)->retraceCallbackArg = (arg);             \
    } while (0)

/**
 * Unsets the current padmgr retrace callback if it and the argument are the same as the ones already registered.
 *
 * @param callback the callback to unset, if it is set
 * @param arg the argument to unset, if it is set
 *
 * @see PADMGR_SET_RETRACE_CALLACK
 */
#define PADMGR_UNSET_RETRACE_CALLACK(padmgr, callback, arg)                                 \
    if ((padmgr)->retraceCallback == (callback) && (padmgr)->retraceCallbackArg == (arg)) { \
        (padmgr)->retraceCallback = NULL;                                                   \
        (padmgr)->retraceCallbackArg = NULL;                                                \
    }                                                                                       \
    (void)0

extern PadMgr gPadMgr;

#endif

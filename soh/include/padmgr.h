#ifndef PADMGR_H
#define PADMGR_H

#include <libultraship/libultra.h>
#include "libu64/pad.h"
#include "irqmgr.h"

#include "extern_c_helper.h"

// TODO: ControllerPakType enum

typedef struct PadMgr {
    /* 0x0000 */ OSContStatus padStatus[4];
    /* 0x0010 */ OSMesg serialMsgBuf[1];
    /* 0x0014 */ OSMesg lockMsgBuf[1];
    /* 0x0018 */ OSMesg interruptMsgBuf[4];
    /* 0x0028 */ OSMesgQueue serialMsgQ;
    /* 0x0040 */ OSMesgQueue lockMsgQ;
    /* 0x0058 */ OSMesgQueue interruptMsgQ;
    /* 0x0070 */ IrqMgrClient irqClient;
    /* 0x0078 */ IrqMgr* irqMgr;
    /* 0x0080 */ OSThread thread;
    /* 0x0230 */ Input inputs[4];
    /* 0x0290 */ OSContPad pads[4];
    /* 0x02A8 */ vu8 validCtrlrsMask;
    /* 0x02A9 */ u8 nControllers;
    /* 0x02AA */ u8 ctrlrIsConnected[4]; // "Key_switch" originally
    /* 0x02AE */ u8 pakType[4]; // 1 if rumble pack, 2 if mempak?
    /* 0x02B2 */ vu8 rumbleEnable[4];
    /* 0x02B6 */ u8 rumbleCounter[4]; // not clear exact meaning
    /* 0x02BC */ OSPfs pfs[4];
    /* 0x045C */ vu8 rumbleOffFrames;
    /* 0x045D */ vu8 rumbleOnFrames;
    /* 0x045E */ u8 preNMIShutdown;
    /* 0x0460 */ void (*retraceCallback)(struct PadMgr* padmgr, s32 unk464);
    /* 0x0464 */ u32 retraceCallbackValue;
} PadMgr; // size = 0x468

// Initialization

EXTERN_C void PadMgr_Init(PadMgr* padmgr, OSMesgQueue* siIntMsgQ, IrqMgr* irqMgr, OSId id, OSPri priority, void* stack);

// Fetching inputs

// This function cannot be prototyped here in all configurations because it is called incorrectly in fault_gc.c
// (see bug in `Fault_PadCallback`)
#if true//PLATFORM_N64 || defined(AVOID_UB)
EXTERN_C void PadMgr_RequestPadData(PadMgr* padmgr, Input* inputs, s32 gameRequest);
#endif

// For internal use by Controller Pak systems
// TODO: Rename to upstream's names
EXTERN_C OSMesgQueue* PadMgr_LockSerialMesgQueue(PadMgr* padmgr);
EXTERN_C void PadMgr_UnlockSerialMesgQueue(PadMgr* padmgr, OSMesgQueue* ctrlrqueue);

EXTERN_C void PadMgr_RumbleStop(PadMgr* padMgr);
EXTERN_C void PadMgr_RumbleReset(PadMgr* padMgr);
EXTERN_C void PadMgr_RumbleSetSingle(PadMgr* padMgr, u32 port, u32 rumble);
EXTERN_C void PadMgr_RumbleSet(PadMgr* padMgr, u8* enable);

// TODO: PADMGR macros

EXTERN_C PadMgr gPadMgr;

#endif
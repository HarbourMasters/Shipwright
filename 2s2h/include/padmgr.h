#ifndef PADMGR_H
#define PADMGR_H

#include "libc/stdbool.h"
#include "ultra64.h"
#include "irqmgr.h"
#include "padutils.h"

typedef enum {
    /*  0 */ PADMGR_CONT_NONE,
    /*  1 */ PADMGR_CONT_NORMAL,
    /*  3 */ PADMGR_CONT_MOUSE = 3,
    /*  4 */ PADMGR_CONT_VOICE_PLUGGED, // VRU plugged but not initialized
    /*  5 */ PADMGR_CONT_VOICE,
    /* -1 */ PADMGR_CONT_UNK = 0xFF
} ControllerDeviceType;

typedef enum {
    /* 0 */ CONT_PAK_NONE,
    /* 1 */ CONT_PAK_RUMBLE,
    /* 2 */ CONT_PAK_OTHER
} ControllerPakType;

typedef struct {
    /* 0x000 */ u8 validCtrlrsMask;
    /* 0x004 */ void (*rumbleRetraceCallback)(void*);
    /* 0x008 */ void* rumbleRetraceArg;
    /* 0x00C */ void (*inputRetraceCallback)(void*);
    /* 0x010 */ void* inputRetraceArg;
    /* 0x014 */ OSContStatus padStatus[MAXCONTROLLERS];
    /* 0x024 */ OSMesg serialMsg;
    /* 0x028 */ OSMesg lockMsg;
    /* 0x02C */ OSMesg interruptMsgBuf[8];
    /* 0x04C */ OSMesgQueue serialLockQueue;
    /* 0x064 */ OSMesgQueue lockQueue;
    /* 0x07C */ OSMesgQueue interruptQueue;
    /* 0x094 */ IrqMgrClient irqClient;
    /* 0x09C */ IrqMgr* irqMgr;
    /* 0x0A0 */ OSThread thread;
    /* 0x250 */ Input inputs[MAXCONTROLLERS];
    /* 0x2B0 */ OSContPad pads[MAXCONTROLLERS];
    /* 0x2C8 */ u8 nControllers;
    /* 0x2C9 */ u8 ctrlrType[MAXCONTROLLERS]; // type of controller currently connected to each port
    /* 0x2CD */ u8 pakType[MAXCONTROLLERS]; // type of controller pak for each port (if applicable)
    /* 0x2D1 */ u8 rumbleEnable[MAXCONTROLLERS]; // whether rumble is active for each port
    /* 0x2D5 */ u8 rumbleTimer[MAXCONTROLLERS];
    /* 0x2DC */ OSPfs rumblePfs[MAXCONTROLLERS];
    /* 0x47C */ vu8 rumbleOffTimer; // amount of VI retraces to not rumble for, takes priority over rumbleOnTimer
    /* 0x47D */ vu8 rumbleOnTimer; // amount of VI retraces to rumble for
    /* 0x47E */ u8 isResetting;
} PadMgr; // size = 0x480

// Initialization

s32 PadSetup_Init(OSMesgQueue* mq, u8* outMask, OSContStatus* status);
void PadMgr_Init(OSMesgQueue* siEvtQ, IrqMgr* irqMgr, OSId threadId, OSPri pri, void* stack);

// Fetching inputs

void PadMgr_GetInputNoLock(Input* inputs, s32 gameRequest);
void PadMgr_GetInput(Input* inputs, s32 gameRequest);
void PadMgr_GetInput2(Input* inputs, s32 gameRequest);

// Callbacks

void PadMgr_SetRumbleRetraceCallback(void (*callback)(void*), void* arg);
void PadMgr_UnsetRumbleRetraceCallback(void (*callback)(void*), void* arg);

void PadMgr_SetInputRetraceCallback(void (*callback)(void*), void* arg);
void PadMgr_UnsetInputRetraceCallback(void (*callback)(void*), void* arg);

// Valid controllers

u8 PadMgr_GetValidControllersMask(void);

// Voice queue

OSMesgQueue* PadMgr_VoiceAcquireSerialEventQueue(void);
void PadMgr_VoiceReleaseSerialEventQueue(OSMesgQueue* serialEventQueue);

// Rumble

void PadMgr_RumbleStop(void);
void PadMgr_RumblePause(void);
void PadMgr_RumbleSetSingle(s32 port, s32 enable);
void PadMgr_RumbleSet(u8 enable[MAXCONTROLLERS]);
s32 PadMgr_ControllerHasRumblePak(s32 port);

#endif

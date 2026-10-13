#ifndef FAULT_H
#define FAULT_H

#include "ultra64.h"
#include "unk.h"
#include "libc/stdarg.h"
#include "libc/stdint.h"
#include "PR/controller.h"
#include "padmgr.h"
#include "stack.h"

// These are the same as the 3-bit ansi color codes
#define FAULT_COLOR_BLACK      0
#define FAULT_COLOR_RED        1
#define FAULT_COLOR_GREEN      2
#define FAULT_COLOR_YELLOW     3
#define FAULT_COLOR_BLUE       4
#define FAULT_COLOR_MAGENTA    5
#define FAULT_COLOR_CYAN       6
#define FAULT_COLOR_WHITE      7
// Additional color codes
#define FAULT_COLOR_DARK_GRAY  8
#define FAULT_COLOR_LIGHT_GRAY 9

#define FAULT_COLOR_STRINGIFY(s) #s
#define FAULT_COLOR_EXPAND_AND_STRINGIFY(s) FAULT_COLOR_STRINGIFY(s)

#define FAULT_ESC '\x1A'
#define FAULT_COLOR(n) "\x1A" FAULT_COLOR_EXPAND_AND_STRINGIFY(FAULT_COLOR_ ## n)


// Address at the end of "non-expansion" memory space
#define FAULT_FB_ADDRESS (void*)((PHYS_TO_K0(0x400000) - SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(u16)))

typedef void (*FaultClientCallback)(void*, void*);

typedef struct FaultClient {
    /* 0x0 */ struct FaultClient* next;
    /* 0x4 */ FaultClientCallback callback;
    /* 0x8 */ void* arg0;
    /* 0xC */ void* arg1;
} FaultClient; // size = 0x10


typedef uintptr_t (*FaultAddrConvClientCallback)(uintptr_t, void*);

typedef struct FaultAddrConvClient {
    /* 0x0 */ struct FaultAddrConvClient* next;
    /* 0x4 */ FaultAddrConvClientCallback callback;
    /* 0x8 */ void* arg;
} FaultAddrConvClient; // size = 0xC

typedef void(*FaultPadCallback)(Input* input);


// Initialization

void Fault_Init(void);

// Fatal Errors

void Fault_AddHungupAndCrashImpl(const char* exp1, const char* exp2);
void Fault_AddHungupAndCrash(const char* file, s32 line);

// Client Registration

void Fault_AddClient(FaultClient* client, FaultClientCallback callback, void* arg0, void* arg1);
void Fault_RemoveClient(FaultClient* client);
void Fault_AddAddrConvClient(FaultAddrConvClient* client, FaultAddrConvClientCallback callback, void* arg);
void Fault_RemoveAddrConvClient(FaultAddrConvClient* client);

// For use in Fault Client callbacks

void Fault_WaitForInput(void);
void Fault_FillScreenBlack(void);
void Fault_SetFrameBuffer(void* fb, u16 w, u16 h);

void FaultDrawer_SetForeColor(u16 color);
void FaultDrawer_SetBackColor(u16 color);
void FaultDrawer_SetFontColor(u16 color);
void FaultDrawer_SetCharPad(s8 padW, s8 padH);
void FaultDrawer_SetCursor(s32 x, s32 y);
s32 FaultDrawer_VPrintf(const char* fmt, va_list ap);
s32 FaultDrawer_Printf(const char* fmt, ...);
void FaultDrawer_DrawText(s32 x, s32 y, const char* fmt, ...);


typedef struct FaultMgr {
    /* 0x000 */ OSThread thread;
    /* 0x1B0 */ STACK(stack, 0x600); // Seems leftover from an earlier version. The thread actually uses a stack of this size at 0x8009BE60
    /* 0x7B0 */ OSMesgQueue queue;
    /* 0x7C8 */ OSMesg msg[1];
    /* 0x7CC */ u8 exit;
    /* 0x7CD */ u8 msgId; // 1 - CPU Break; 2 - Fault; 3 - Unknown
    /* 0x7CE */ u8 faultHandlerEnabled;
    /* 0x7CF */ u8 autoScroll;
    /* 0x7D0 */ OSThread* faultedThread;
    /* 0x7D4 */ FaultPadCallback padCallback;
    /* 0x7D8 */ FaultClient* clients;
    /* 0x7DC */ FaultAddrConvClient* addrConvClients;
    /* 0x7E0 */ UNK_TYPE1 unk_7E0[0x4];
    /* 0x7E4 */ Input inputs[MAXCONTROLLERS];
    /* 0x844 */ void* fb;
} FaultMgr; // size = 0x848

// TODO: Commented out to try to avoid bss reorder in fault.c
// extern FaultMgr gFaultMgr;

#endif

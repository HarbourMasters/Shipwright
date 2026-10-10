#ifndef PR_OS_SYSTEM_H
#define PR_OS_SYSTEM_H

#include "ultratypes.h"

/*
 * Values for osTvType
 */
#define OS_TV_PAL 0
#define OS_TV_NTSC 1
#define OS_TV_MPAL 2

/*
 * Size of buffer the retains contents after NMI
 */
#define OS_APP_NMI_BUFSIZE 64

extern s32 osTvType;
extern s32 osRomType;
extern void* osRomBase;
extern s32 osResetType;
extern s32 osCicId;
extern s32 osVersion;
extern u32 osMemSize;
extern s32 osAppNMIBuffer[8];

extern u64 osClockRate;

extern s32 osViClock;

extern u32 __OSGlobalIntMask;

u32 osGetMemSize(void);
s32 osAfterPreNMI(void);

#endif
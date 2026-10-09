#ifndef PR_OS_PI_H
#define PR_OS_PI_H

#include <libultraship/libultra/pi.h>

#if 0
#include "ultratypes.h"
#include "os_message.h"
#include "libc/stddef.h"
#include "libc/stdint.h"


typedef struct {
    /* 0x00 */ u32 errStatus;
    /* 0x04 */ void* dramAddr;
    /* 0x08 */ void* C2Addr;
    /* 0x0C */ u32 sectorSize;
    /* 0x10 */ u32 C1ErrNum;
    /* 0x14 */ u32 C1ErrSector[4];
} __OSBlockInfo; // size = 0x24

typedef struct {
    /* 0x00 */ u32 cmdType;
    /* 0x04 */ u16 transferMode;
    /* 0x06 */ u16 blockNum;
    /* 0x08 */ s32 sectorNum;
    /* 0x0C */ uintptr_t devAddr;
    /* 0x10 */ u32 bmCtlShadow;
    /* 0x14 */ u32 seqCtlShadow;
    /* 0x18 */ __OSBlockInfo block[2];
} __OSTranxInfo; // size = 0x60

typedef struct OSPiHandle {
    /* 0x00 */ struct OSPiHandle* next;
    /* 0x04 */ u8 type;
    /* 0x05 */ u8 latency;
    /* 0x06 */ u8 pageSize;
    /* 0x07 */ u8 relDuration;
    /* 0x08 */ u8 pulse;
    /* 0x09 */ u8 domain;
    /* 0x0C */ uintptr_t baseAddress;
    /* 0x10 */ u32 speed;
    /* 0x14 */ __OSTranxInfo transferInfo;
} OSPiHandle; // size = 0x74

typedef struct {
    /* 0x0 */ u8 type;
    /* 0x4 */ uintptr_t address;
} OSPiInfo; // size = 0x8

typedef struct {
    /* 0x0 */ u16 type;
    /* 0x2 */ u8 pri;
    /* 0x3 */ u8 status;
    /* 0x4 */ OSMesgQueue* retQueue;
} OSIoMesgHdr; // size = 0x8

typedef struct {
    /* 0x00 */ OSIoMesgHdr hdr;
    /* 0x08 */ void* dramAddr;
    /* 0x0C */ uintptr_t devAddr;
    /* 0x10 */ size_t size;
    /* 0x14 */ OSPiHandle* piHandle;
} OSIoMesg; // size = 0x88

typedef struct OSDevMgr {
    /* 0x00 */ s32 active;
    /* 0x04 */ OSThread* thread;
    /* 0x08 */ OSMesgQueue* cmdQueue;
    /* 0x0C */ OSMesgQueue* evtQueue;
    /* 0x10 */ OSMesgQueue* acsQueue;
    /* 0x14 */ s32 (*piDmaCallback)(s32, uintptr_t, void*, size_t);
    /* 0x18 */ s32 (*epiDmaCallback)(OSPiHandle*, s32, uintptr_t, void*, size_t);
} OSDevMgr; // size = 0x1C

#define OS_READ 0
#define OS_WRITE 1

/*
 * I/O message types
 */
#define OS_MESG_TYPE_BASE 10
#define OS_MESG_TYPE_LOOPBACK (OS_MESG_TYPE_BASE + 0)
#define OS_MESG_TYPE_DMAREAD (OS_MESG_TYPE_BASE + 1)
#define OS_MESG_TYPE_DMAWRITE (OS_MESG_TYPE_BASE + 2)
#define OS_MESG_TYPE_VRETRACE (OS_MESG_TYPE_BASE + 3)
#define OS_MESG_TYPE_COUNTER (OS_MESG_TYPE_BASE + 4)
#define OS_MESG_TYPE_EDMAREAD (OS_MESG_TYPE_BASE + 5)
#define OS_MESG_TYPE_EDMAWRITE (OS_MESG_TYPE_BASE + 6)

/*
 * I/O message priority
 */
#define OS_MESG_PRI_NORMAL 0
#define OS_MESG_PRI_HIGH 1

/*
 * PI/EPI
 */
#define PI_DOMAIN1 0
#define PI_DOMAIN2 1

extern OSPiHandle* __osPiTable;

#endif
void osCreatePiManager(OSPri pri, OSMesgQueue* cmdQ, OSMesg* cmdBuf, s32 cmdMsgCnt);

OSPiHandle* osCartRomInit(void);

s32 osEPiWriteIo(OSPiHandle* handle, uintptr_t devAddr, u32 data);
s32 osEPiReadIo(OSPiHandle* handle, uintptr_t devAddr, u32* data);
s32 osEPiStartDma(OSPiHandle* pihandle, OSIoMesg* mb, s32 direction);
s32 osEPiLinkHandle(OSPiHandle* handle);
#endif

#ifndef PR_OS_FLASH_H
#define PR_OS_FLASH_H

#include "ultratypes.h"
#include "os_pi.h"

#define FLASH_START_ADDR 0x08000000           // FRAM Base Address in Cart Memory

#define FLASH_SIZE 0x20000

#define FLASH_LATENCY       0x5
#define FLASH_PULSE         0x0C
#define FLASH_PAGE_SIZE     0xF
#define FLASH_REL_DURATION  0x2
#define DEVICE_TYPE_FLASH   8

/* OLD_FLASH is MX_PROTO_A, MX_A and MX_C */
#define OLD_FLASH   0
/* NEW_FLASH is MX_B_AND_D and MATSUSHITA flash */
#define NEW_FLASH   1

#define FLASH_STATUS_ERASE_BUSY     2
#define FLASH_STATUS_ERASE_OK       0
#define FLASH_STATUS_ERASE_ERROR    -1

#define FLASH_STATUS_WRITE_BUSY     1
#define FLASH_STATUS_WRITE_OK       0
#define FLASH_STATUS_WRITE_ERROR    -1

#define FLASH_VERSION_MX_PROTO_A 0x00C20000
#define FLASH_VERSION_MX_A       0x00C20001
#define FLASH_VERSION_MX_C       0x00C2001E
#define FLASH_VERSION_MX_B_AND_D 0x00C2001D
#define FLASH_VERSION_MEI        0x003200F1


OSPiHandle* osFlashReInit(u8 latency, u8 pulse, u8 pageSize, u8 relDuration, u32 start);
OSPiHandle* osFlashInit(void);
void osFlashReadStatus(u8* flashStatus);
void osFlashReadId(u32* flashType, u32* flashVendor);
void osFlashClearStatus(void);
s32 osFlashAllErase(void);
s32 osFlashSectorErase(u32 pageNum);
s32 osFlashWriteBuffer(OSIoMesg* mb, s32 priority, void* dramAddr, OSMesgQueue* mq);
s32 osFlashWriteArray(u32 pageNum);
s32 osFlashReadArray(OSIoMesg* mb, s32 priority, u32 pageNum, void* dramAddr, u32 pageCount, OSMesgQueue* mq);
void osFlashChange(u32 flashNum);
void osFlashAllEraseThrough(void);
void osFlashSectorEraseThrough(u32 pageNum);
s32 osFlashCheckEraseEnd(void);

#endif

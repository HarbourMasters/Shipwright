#include "ultra64.h"
#include "PR/os_internal_flash.h"
#include "alignment.h"
#include "macros.h"

u32 __osFlashID[4] ALIGNED(8);
OSIoMesg __osFlashMsg ALIGNED(8);
OSMesgQueue __osFlashMessageQ ALIGNED(8);
OSPiHandle __osFlashHandler ALIGNED(8);
OSMesg __osFlashMsgBuf[1];
s32 __osFlashVersion;
static s32 sBssPad[5];

uintptr_t osFlashGetAddr(u32 pageNum) {
    uintptr_t addr;

    if (__osFlashVersion == OLD_FLASH) {
        // Account for hardware bug in old flash where the address bits are shifted 1-off where they should be
        addr = pageNum * (FLASH_BLOCK_SIZE >> 1);
    } else {
        addr = pageNum * FLASH_BLOCK_SIZE;
    }

    return addr;
}

OSPiHandle* osFlashReInit(u8 latency, u8 pulse, u8 pageSize, u8 relDuration, u32 start) {
    __osFlashHandler.baseAddress = PHYS_TO_K1(start);
    __osFlashHandler.type++;
    __osFlashHandler.latency = latency;
    __osFlashHandler.pulse = pulse;
    __osFlashHandler.pageSize = pageSize;
    __osFlashHandler.relDuration = relDuration;
    __osFlashHandler.domain = PI_DOMAIN2;

    return &__osFlashHandler;
}

void osFlashChange(u32 flashNum) {
    __osFlashHandler.baseAddress = PHYS_TO_K1(FLASH_START_ADDR + (flashNum * FLASH_SIZE));
    __osFlashHandler.type = DEVICE_TYPE_FLASH + flashNum;

    return;
}

OSPiHandle* osFlashInit(void) {
    u32 flashType;
    u32 flashVendor;

    osCreateMesgQueue(&__osFlashMessageQ, __osFlashMsgBuf, ARRAY_COUNT(__osFlashMsgBuf));

    if (__osFlashHandler.baseAddress == PHYS_TO_K1(FLASH_START_ADDR)) {
        return &__osFlashHandler;
    }

    __osFlashHandler.type = DEVICE_TYPE_FLASH;
    __osFlashHandler.baseAddress = PHYS_TO_K1(FLASH_START_ADDR);
    __osFlashHandler.latency = FLASH_LATENCY;
    __osFlashHandler.pulse = FLASH_PULSE;
    __osFlashHandler.pageSize = FLASH_PAGE_SIZE;
    __osFlashHandler.relDuration = FLASH_REL_DURATION;
    __osFlashHandler.domain = PI_DOMAIN2;
    __osFlashHandler.speed = 0;

    bzero(&__osFlashHandler.transferInfo, sizeof(__OSTranxInfo));

    osEPiLinkHandle(&__osFlashHandler);
    osFlashReadId(&flashType, &flashVendor);

    if ((flashVendor == FLASH_VERSION_MX_C) || (flashVendor == FLASH_VERSION_MX_A) ||
        (flashVendor == FLASH_VERSION_MX_PROTO_A)) {
        __osFlashVersion = OLD_FLASH;
    } else {
        __osFlashVersion = NEW_FLASH;
    }

    return &__osFlashHandler;
}

void osFlashReadStatus(u8* flashStatus) {
    u32 outFlashStatus;

    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_STATUS);
    // read status using IO
    osEPiReadIo(&__osFlashHandler, __osFlashHandler.baseAddress, &outFlashStatus);

    // why twice ?
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_STATUS);
    osEPiReadIo(&__osFlashHandler, __osFlashHandler.baseAddress, &outFlashStatus);

    *flashStatus = outFlashStatus & 0xFF;

    return;
}

void osFlashReadId(u32* flashType, u32* flashVendor) {
    u8 flashStatus;

    // why read status?
    osFlashReadStatus(&flashStatus);

    // select silicon id read mode
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_ID);

    // read silicon id using DMA
    __osFlashMsg.hdr.pri = OS_MESG_PRI_NORMAL;
    __osFlashMsg.hdr.retQueue = &__osFlashMessageQ;
    __osFlashMsg.dramAddr = __osFlashID;
    __osFlashMsg.devAddr = 0;
    __osFlashMsg.size = 2 * sizeof(u32);

    osInvalDCache(__osFlashID, sizeof(__osFlashID));
    osEPiStartDma(&__osFlashHandler, &__osFlashMsg, OS_READ);
    osRecvMesg(&__osFlashMessageQ, NULL, OS_MESG_BLOCK);

    *flashType = __osFlashID[0];
    *flashVendor = __osFlashID[1];

    return;
}

void osFlashClearStatus(void) {
    // select status mode
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_STATUS);
    // clear status
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress, 0);

    return;
}

s32 osFlashAllErase(void) {
    u32 status;
    OSTimer timer;
    OSMesgQueue mq;
    OSMesg msg;

    // start chip erase operation
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_CHIP_ERASE);
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_EXECUTE_ERASE);

    // wait for completion by polling erase-busy flag
    osCreateMesgQueue(&mq, &msg, 1);
    do {
        osSetTimer(&timer, OS_USEC_TO_CYCLES(15000), 0, &mq, &msg);
        osRecvMesg(&mq, &msg, OS_MESG_BLOCK);
        osEPiReadIo(&__osFlashHandler, __osFlashHandler.baseAddress, &status);
    } while ((status & FLASH_STATUS_ERASE_BUSY) == FLASH_STATUS_ERASE_BUSY);

    // check erase operation status, clear status
    osEPiReadIo(&__osFlashHandler, __osFlashHandler.baseAddress, &status);
    osFlashClearStatus();

    if (((status & 0xFF) == 0x08) || ((status & 0xFF) == 0x48) || ((status & 0x08) == 0x08)) {
        return FLASH_STATUS_ERASE_OK;
    } else {
        return FLASH_STATUS_ERASE_ERROR;
    }
}

void osFlashAllEraseThrough(void) {
    // start chip erase operation
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_CHIP_ERASE);
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_EXECUTE_ERASE);
}

s32 osFlashCheckEraseEnd(void) {
    u8 status;

    // check if erase operation is completed
    osFlashReadStatus(&status);
    if ((status & FLASH_STATUS_ERASE_BUSY) == FLASH_STATUS_ERASE_BUSY) {
        return FLASH_STATUS_ERASE_BUSY;
    } else {
        // check erase operation status, clear status
        osFlashReadStatus(&status);
    }
    osFlashClearStatus();

    if (((status & 0xFF) == 0x08) || ((status & 0xFF) == 0x48) || ((status & 0x08) == 0x08)) {
        return FLASH_STATUS_ERASE_OK;
    } else {
        return FLASH_STATUS_ERASE_ERROR;
    }
}

s32 osFlashSectorErase(u32 pageNum) {
    u32 status;
    OSTimer timer;
    OSMesgQueue mq;
    OSMesg msg;

    // start sector erase operation
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_SECTOR_ERASE | pageNum);
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_EXECUTE_ERASE);

    // wait for completion by polling erase-busy flag
    osCreateMesgQueue(&mq, &msg, 1);
    do {
        osSetTimer(&timer, OS_USEC_TO_CYCLES(12500), 0, &mq, &msg);
        osRecvMesg(&mq, &msg, OS_MESG_BLOCK);
        osEPiReadIo(&__osFlashHandler, __osFlashHandler.baseAddress, &status);
    } while ((status & FLASH_STATUS_ERASE_BUSY) == FLASH_STATUS_ERASE_BUSY);

    // check erase operation status, clear status
    osEPiReadIo(&__osFlashHandler, __osFlashHandler.baseAddress, &status);
    osFlashClearStatus();

    if (((status & 0xFF) == 0x08) || ((status & 0xFF) == 0x48) || ((status & 0x08) == 0x08)) {
        return FLASH_STATUS_ERASE_OK;
    } else {
        return FLASH_STATUS_ERASE_ERROR;
    }
}

void osFlashSectorEraseThrough(u32 pageNum) {
    // start sector erase operation
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_SECTOR_ERASE | pageNum);
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_EXECUTE_ERASE);
}

s32 osFlashWriteBuffer(OSIoMesg* mb, s32 priority, void* dramAddr, OSMesgQueue* mq) {
    s32 ret;

    // select load page mode
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_PAGE_PROGRAM);

    // DMA 128-byte page
    mb->hdr.pri = priority;
    mb->hdr.retQueue = mq;
    mb->dramAddr = dramAddr;
    mb->devAddr = 0;
    mb->size = FLASH_BLOCK_SIZE;

    ret = osEPiStartDma(&__osFlashHandler, mb, OS_WRITE);

    return ret;
}

s32 osFlashWriteArray(u32 pageNum) {
    u32 status;
    OSTimer timer;
    OSMesgQueue mq;
    OSMesg msg;

    // only needed for new flash ?
    if (__osFlashVersion == NEW_FLASH) {
        osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_PAGE_PROGRAM);
    }

    // start program page operation
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_PROGRAM_PAGE | pageNum);

    // wait for completion by polling write-busy flag
    osCreateMesgQueue(&mq, &msg, 1);
    do {
        osSetTimer(&timer, OS_USEC_TO_CYCLES(200), 0, &mq, &msg);
        osRecvMesg(&mq, &msg, OS_MESG_BLOCK);
        osEPiReadIo(&__osFlashHandler, __osFlashHandler.baseAddress, &status);
    } while ((status & FLASH_STATUS_WRITE_BUSY) == FLASH_STATUS_WRITE_BUSY);

    // check program operation status, clear status
    osEPiReadIo(&__osFlashHandler, __osFlashHandler.baseAddress, &status);
    osFlashClearStatus();

    if (((status & 0xFF) == 0x04) || ((status & 0xFF) == 0x44) || ((status & 0x04) == 0x04)) {
        return FLASH_STATUS_WRITE_OK;
    } else {
        return FLASH_STATUS_WRITE_ERROR;
    }
}

s32 osFlashReadArray(OSIoMesg* mb, s32 priority, u32 pageNum, void* dramAddr, u32 pageCount, OSMesgQueue* mq) {
    s32 ret;
    u32 dummy;
    u32 last_page;
    u32 pages;

    // select read array mode
    osEPiWriteIo(&__osFlashHandler, __osFlashHandler.baseAddress | FLASH_CMD_REG, FLASH_CMD_READ_ARRAY);

    // dummy read to initiate "fast-page" reads ?
    osEPiReadIo(&__osFlashHandler, __osFlashHandler.baseAddress, &dummy);

    // DMA requested pages
    mb->hdr.pri = priority;
    mb->hdr.retQueue = mq;
    mb->dramAddr = dramAddr;

    last_page = pageNum + pageCount - 1;

    if ((last_page & 0xF00) != (pageNum & 0xF00)) {
        pages = 256 - (pageNum & 0xFF);
        pageCount -= pages;
        mb->size = pages * FLASH_BLOCK_SIZE;
        mb->devAddr = osFlashGetAddr(pageNum);
        osEPiStartDma(&__osFlashHandler, mb, OS_READ);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);
        pageNum = (pageNum + 256) & 0xF00;
        mb->dramAddr = (void*)((uintptr_t)mb->dramAddr + mb->size);
    }

    while (pageCount > 256) {
        pages = 256;
        pageCount -= 256;
        mb->size = pages * FLASH_BLOCK_SIZE;
        mb->devAddr = osFlashGetAddr(pageNum);
        osEPiStartDma(&__osFlashHandler, mb, OS_READ);
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);
        pageNum += 256;
        mb->dramAddr = (void*)((uintptr_t)mb->dramAddr + mb->size);
    }

    mb->size = pageCount * FLASH_BLOCK_SIZE;
    mb->devAddr = osFlashGetAddr(pageNum);
    ret = osEPiStartDma(&__osFlashHandler, mb, OS_READ);

    return ret;
}

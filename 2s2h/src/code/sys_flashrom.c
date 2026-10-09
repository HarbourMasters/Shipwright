#include "prevent_bss_reordering.h"
#include "global.h"
#include "fault.h"
#include "stack.h"
#include "stackcheck.h"
#include "system_malloc.h"
#include "z64thread.h"
#include "sys_flashrom.h"
#include "PR/os_internal_flash.h"
#include "2s2h/SaveManager/SaveManager.h"

OSMesgQueue sFlashromMesgQueue;
OSMesg sFlashromMesg[1];
s32 sFlashromIsInit;
s32 sFlashromVendor;

STACK(sSysFlashromStack, 0x1000);
StackEntry sSysFlashromStackInfo;
OSThread sSysFlashromThread;
FlashromRequest sFlashromRequest;
OSMesg sSysFlashromMsgBuf[1];

s32 SysFlashrom_IsInit(void) {
    return sFlashromIsInit;
}

const char* SysFlashrom_GetVendorStr(void) {
    switch (sFlashromVendor) {
        case FLASH_VERSION_MX_PROTO_A:
            return "PROTO A";

        case FLASH_VERSION_MX_A:
            return "A";

        case FLASH_VERSION_MX_C:
            return "C";

        case FLASH_VERSION_MEI:
            return "D(NEW)";

        case FLASH_VERSION_MX_B_AND_D:
            return "B or D";

        default:
            return "UNKNOWN";
    }
}

s32 SysFlashrom_CheckFlashType(void) {
    u32 flashType;
    u32 flashVendor;

    if (!SysFlashrom_IsInit()) {
        return -1;
    }
    osFlashReadId(&flashType, &flashVendor);
    sFlashromVendor = flashVendor;
    if (flashType == FLASH_TYPE_MAGIC) {
        if ((flashVendor == FLASH_VERSION_MX_PROTO_A) || (flashVendor == FLASH_VERSION_MX_A) ||
            (flashVendor == FLASH_VERSION_MX_C) || (flashVendor == FLASH_VERSION_MEI) ||
            (flashVendor == FLASH_VERSION_MX_B_AND_D)) {
            return 0;
        } else {
            return -1;
        }
    }
    return -1;
}

s32 SysFlashrom_InitFlash(void) {
    osCreateMesgQueue(&sFlashromMesgQueue, sFlashromMesg, ARRAY_COUNT(sFlashromMesg));
    osFlashInit();
    sFlashromIsInit = true;
    if (SysFlashrom_CheckFlashType() != 0) {
        sFlashromIsInit = false;
        return -1;
    }
    return 0;
}

s32 SysFlashrom_ReadData(void* addr, u32 pageNum, u32 pageCount) {
    // #region 2S2H [Port] Redirect to our own read function
    return SaveManager_SysFlashrom_ReadData(addr, pageNum, pageCount);
    // #endregion

    OSIoMesg msg;

    if (!SysFlashrom_IsInit()) {
        return -1;
    }
    osInvalDCache(addr, pageCount * FLASH_BLOCK_SIZE);
    osFlashReadArray(&msg, OS_MESG_PRI_NORMAL, pageNum, addr, pageCount, &sFlashromMesgQueue);
    osRecvMesg(&sFlashromMesgQueue, NULL, OS_MESG_BLOCK);
    return 0;
}

s32 SysFlashrom_EraseSector(u32 page) {
    if (!SysFlashrom_IsInit()) {
        return -1;
    }
    return osFlashSectorErase(page);
}

s32 SysFlashrom_ExecWrite(void* addr, u32 pageNum, u32 pageCount) {
    OSIoMesg msg;
    s32 result;
    u32 i;

    if (!SysFlashrom_IsInit()) {
        return -1;
    }
    // Ensure the page is always aligned to a sector boundary.
    if ((pageNum % FLASH_BLOCK_SIZE) != 0) {
        Fault_AddHungupAndCrash("../sys_flashrom.c", 275);
    }
    osWritebackDCache(addr, pageCount * FLASH_BLOCK_SIZE);
    for (i = 0; i < pageCount; i++) {
        osFlashWriteBuffer(&msg, OS_MESG_PRI_NORMAL, (u8*)addr + i * FLASH_BLOCK_SIZE, &sFlashromMesgQueue);
        osRecvMesg(&sFlashromMesgQueue, NULL, OS_MESG_BLOCK);
        result = osFlashWriteArray(i + pageNum);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

s32 SysFlashrom_AttemptWrite(void* addr, u32 pageNum, u32 pageCount) {
    s32 result;
    s32 i;

    if (!SysFlashrom_IsInit()) {
        return -1;
    }
    osWritebackDCache(addr, pageCount * FLASH_BLOCK_SIZE);
    i = 0;
failRetry:
    result = SysFlashrom_EraseSector(pageNum);
    if (result != 0) {
        if (i < 3) {
            i++;
            goto failRetry;
        }
        return result;
    }
    result = SysFlashrom_ExecWrite(addr, pageNum, pageCount);
    if (result != 0) {
        if (i < 3) {
            i++;
            goto failRetry;
        }
        return result;
    }
    return 0;
}

// Flash bits can only by flipped with a write from 1 -> 0. Going from 0 -> 1 requires an erasure.
// This function will try to determine if any sectors need to be erased before writing saving erase cycles.
s32 SysFlashrom_NeedsToErase(void* data, void* addr, u32 pageCount) {
    u32 size;
    u32 i;

    for (i = 0; i < pageCount * FLASH_BLOCK_SIZE; i += 4) {
        if ((*(s32*)data & *(s32*)addr) != *(s32*)addr) {
            return false;
        }
    }
    return true;
}

s32 SysFlashrom_WriteData(void* addr, u32 pageNum, u32 pageCount) {
    void* data;
    size_t size;
    s32 ret;

    if (!SysFlashrom_IsInit()) {
        return -1;
    }
    size = pageCount * FLASH_BLOCK_SIZE;
    data = SystemArena_Malloc(size);
    if (data == NULL) {
        ret = SysFlashrom_AttemptWrite(addr, pageNum, pageCount);
    } else {
        SysFlashrom_ReadData(data, pageNum, pageCount);
        if (memcmp(data, addr, size) == 0) {
            ret = 0;
        } else {
            // Will always erase the sector even if it wouldn't normally need to.
            if (SysFlashrom_NeedsToErase(data, addr, pageCount)) {
                ret = SysFlashrom_AttemptWrite(addr, pageNum, pageCount);
            } else {
                ret = SysFlashrom_AttemptWrite(addr, pageNum, pageCount);
            }
            if (ret == 0) {
                SysFlashrom_ReadData(data, pageNum, pageCount);
                if (memcmp(data, addr, size) == 0) {
                    ret = 0;
                } else {
                    ret = -1;
                }
            }
        }
        SystemArena_Free(data);
    }
    return ret;
}

void SysFlashrom_ThreadEntry(void* arg) {
    FlashromRequest* req = (FlashromRequest*)arg;

    switch (req->requestType) {
        case FLASHROM_REQUEST_WRITE:
            req->response = SysFlashrom_WriteData(req->addr, req->pageNum, req->pageCount);
            osSendMesg(&req->messageQueue, OS_MESG_32(req->response), OS_MESG_BLOCK);
            break;

        case FLASHROM_REQUEST_READ:
            req->response = SysFlashrom_ReadData(req->addr, req->pageNum, req->pageCount);
            osSendMesg(&req->messageQueue, OS_MESG_32(req->response), OS_MESG_BLOCK);
            break;
    }
}

void SysFlashrom_WriteDataAsync(u8* addr, u32 pageNum, u32 pageCount) {
    // #region 2S2H [Port] Redirect to our own write function
    SaveManager_SysFlashrom_WriteData(addr, pageNum, pageCount);
    return;
    // #endregion

    FlashromRequest* req = &sFlashromRequest;
    if (SysFlashrom_IsInit()) {
        req->requestType = FLASHROM_REQUEST_WRITE;
        req->addr = addr;
        req->pageNum = pageNum;
        req->pageCount = pageCount;
        osCreateMesgQueue(&req->messageQueue, sSysFlashromMsgBuf, ARRAY_COUNT(sSysFlashromMsgBuf));
        StackCheck_Init(&sSysFlashromStackInfo, sSysFlashromStack, STACK_TOP(sSysFlashromStack), 0, 0x100,
                        "sys_flashrom");
        osCreateThread(&sSysFlashromThread, Z_THREAD_ID_FLASHROM, SysFlashrom_ThreadEntry, req,
                       STACK_TOP(sSysFlashromStack), Z_PRIORITY_FLASHROM);
        osStartThread(&sSysFlashromThread);
    }
}

s32 SysFlashrom_IsBusy(void) {
    // #region 2S2H [Port] This is only checked in 3 places in which we always want to tell it we've "started working"
    // Eventually we should probably have this call into BenPort and check if a save is in progress
    return 1;
    // #endregion

    OSMesgQueue* queue = &sFlashromRequest.messageQueue;

    if (!SysFlashrom_IsInit()) {
        return -1;
    }
    return MQ_IS_FULL(queue);
}

s32 SysFlashrom_AwaitResult(void) {
    // #region 2S2H [Port] Currently all of our save writes/reads are synchronous, so we don't need to wait for anything
    return 0;
    // #endregion

    if (!SysFlashrom_IsInit()) {
        return -1;
    }
    osRecvMesg(&sFlashromRequest.messageQueue, NULL, OS_MESG_BLOCK);
    osDestroyThread(&sSysFlashromThread);
    StackCheck_Cleanup(&sSysFlashromStackInfo);
    return sFlashromRequest.response;
}

void SysFlashrom_WriteDataSync(void* addr, u32 pageNum, u32 pageCount) {
    SysFlashrom_WriteDataAsync(addr, pageNum, pageCount);
    SysFlashrom_AwaitResult();
}

#include "global.h"

typedef void (*arg3_800FC868)(void*);
typedef void (*arg3_800FC8D8)(void*, u32);
typedef void (*arg3_800FC948)(void*, u32, u32, u32, u32, u32, u32, u32, u32);
typedef void (*arg3_800FCA18)(void*, u32);

typedef struct InitFunc {
    s32 nextOffset;
    void (*func)(void);
} InitFunc;

// .data
void* sInitFuncs = NULL;

char sNew[] = { 'n', 'e', 'w' };

char D_80134488[0x18] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x80, 0x00, 0x00,
    0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
};

s32 Overlay_Load(uintptr_t vRomStart, uintptr_t vRomEnd, void* vRamStart, void* vRamEnd, void* allocatedVRamAddr) 
{
    return 0;

#if 0
    s32 pad;
    uintptr_t end;
    u32 bssSize;
    OverlayRelocationSection* ovl;
    u32 relocCnt;
    u32 ovlOffset;
    size_t size;

    if (gOverlayLogSeverity >= 3) {
        // "Start loading dynamic link function"
        osSyncPrintf("\nダイナミックリンクファンクションのロードを開始します\n");
    }

    if (gOverlayLogSeverity >= 3) {
        size = vRomEnd - vRomStart;
        // "DMA transfer of TEXT, DATA, RODATA + rel (%08x-%08x)"
        osSyncPrintf("TEXT,DATA,RODATA+relをＤＭＡ転送します(%08x-%08x)\n", allocatedVRamAddr,
                     (uintptr_t)allocatedVRamAddr + size);
    }

    size = vRomEnd - vRomStart;
    end = (uintptr_t)allocatedVRamAddr + size;
    DmaMgr_SendRequest0(allocatedVRamAddr, vRomStart, size);

    ovlOffset = ((s32*)end)[-1];

    ovl = (OverlayRelocationSection*)((uintptr_t)end - ovlOffset);
    if (gOverlayLogSeverity >= 3) {
        osSyncPrintf("TEXT(%08x), DATA(%08x), RODATA(%08x), BSS(%08x)\n", ovl->textSize, ovl->dataSize, ovl->rodataSize,
                     ovl->bssSize);
    }

    if (gOverlayLogSeverity >= 3) {
        osSyncPrintf("リロケーションします\n"); // "Relocate"
    }

    Overlay_Relocate(allocatedVRamAddr, ovl, vRamStart);

    bssSize = ovl->bssSize;
    if (bssSize != 0) {
        if (gOverlayLogSeverity >= 3) {
            // "Clear BSS area (% 08x-% 08x)"
            osSyncPrintf("BSS領域をクリアします(%08x-%08x)\n", end, end + ovl->bssSize);
        }

        size = ovl->bssSize;
        bssSize = size;
        bzero((void*)end, bssSize);
        relocCnt = ovl->nRelocations;
        (void)relocCnt; // suppresses set but unused warning
    }

    size = (uintptr_t)&ovl->relocations[ovl->nRelocations] - (uintptr_t)ovl;
    if (gOverlayLogSeverity >= 3) {
        // "Clear REL area (%08x-%08x)"
        osSyncPrintf("REL領域をクリアします(%08x-%08x)\n", ovl, (uintptr_t)ovl + size);
    }

    bzero(ovl, size);

    size = (uintptr_t)vRamEnd - (uintptr_t)vRamStart;
    osWritebackDCache(allocatedVRamAddr, size);
    osInvalICache(allocatedVRamAddr, size);

    if (gOverlayLogSeverity >= 3) {
        // "Finish loading dynamic link function"
        osSyncPrintf("ダイナミックリンクファンクションのロードを終了します\n\n");
    }
    return size;
#endif
}

// possibly some kind of new() function
void* func_800FC800(size_t size) {
    if (size == 0) {
        size = 1;
    }

    return __osMallocDebug(&gSystemArena, size, sNew, 0);
}

// possible some kind of delete() function
void func_800FC83C(void* ptr) {
    if (ptr != NULL) {
        __osFree(&gSystemArena, ptr);
    }
}

void func_800FC868(void* blk, u32 nBlk, u32 blkSize, arg3_800FC868 arg3) {
    uintptr_t pos;

    for (pos = (uintptr_t)blk; pos < (uintptr_t)blk + (nBlk * blkSize); pos = (uintptr_t)pos + (blkSize & ~0)) {
        arg3((void*)pos);
    }
}

void func_800FC8D8(void* blk, u32 nBlk, s32 blkSize, arg3_800FC8D8 arg3) {
    uintptr_t pos;

    for (pos = (uintptr_t)blk; pos < (uintptr_t)blk + (nBlk * blkSize); pos = (uintptr_t)pos + (blkSize & ~0)) {
        arg3((void*)pos, 2);
    }
}

void* func_800FC948(void* blk, u32 nBlk, u32 blkSize, arg3_800FC948 arg3) {
    uintptr_t pos;

    if (blk == NULL) {
        blk = func_800FC800(nBlk * blkSize);
    }

    if (blk != NULL && arg3 != NULL) {
        pos = (uintptr_t)blk;
        while (pos < (uintptr_t)blk + (nBlk * blkSize)) {
            arg3((void*)pos, 0, 0, 0, 0, 0, 0, 0, 0);
            pos = (uintptr_t)pos + (blkSize & ~0);
        }
    }
    return blk;
}

void func_800FCA18(void* blk, u32 nBlk, u32 blkSize, arg3_800FCA18 arg3, s32 arg4) {
    uintptr_t pos;
    uintptr_t end;
    s32 masked_arg2;

    if (blk == 0) {
        return;
    }
    if (arg3 != 0) {
        end = (uintptr_t)blk;
        masked_arg2 = (s32)(blkSize & ~0);
        pos = (uintptr_t)end + (nBlk * blkSize);

        if (masked_arg2) {}

        while (pos > end) {
            pos -= masked_arg2;
            arg3((void*)pos, 2);
        }

        if (!masked_arg2) {}
    }

    if (arg4 != 0) {
        func_800FC83C(blk);
    }
}

void func_800FCB34(void) {
    InitFunc* initFunc = (InitFunc*)&sInitFuncs;
    u32 nextOffset = initFunc->nextOffset;
    InitFunc* prev = NULL;

    while (nextOffset != 0) {
        initFunc = (InitFunc*)((s32)initFunc + nextOffset);

        if (initFunc->func != NULL) {
            (*initFunc->func)();
        }

        nextOffset = initFunc->nextOffset;
        initFunc->nextOffset = (s32)prev;
        prev = initFunc;
    }

    sInitFuncs = prev;
}

void SystemHeap_Init(void* start, size_t size) {
    SystemArena_Init(start, size);
    func_800FCB34();
}

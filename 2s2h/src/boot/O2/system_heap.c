/**
 * @file system_heap.c
 *
 * @note:
 *  Only SystemHeap_Init() is used, and is essentially just a wrapper for SystemArena_Init().
 *
 */
#include "global.h"
#include "system_malloc.h"

typedef void (*BlockFunc)(uintptr_t);
typedef void (*BlockFunc1)(uintptr_t, u32);
typedef void (*BlockFunc8)(uintptr_t, u32, u32, u32, u32, u32, u32, u32, u32);

typedef struct InitFunc {
    /* 0x0 */ uintptr_t nextOffset;
    /* 0x4 */ void (*func)(void);
} InitFunc; // size = 0x8

void* sInitFuncs = NULL;

char sNew[] = "";

void* SystemHeap_Malloc(size_t size) {
    if (size == 0) {
        size = 1;
    }

    return __osMalloc(&gSystemArena, size);
}

void SystemHeap_Free(void* ptr) {
    if (ptr != NULL) {
        __osFree(&gSystemArena, ptr);
    }
}

void SystemHeap_RunBlockFunc(void* blk, size_t nBlk, size_t blkSize, BlockFunc blockFunc) {
    uintptr_t pos = (uintptr_t)blk;

    for (; pos < (uintptr_t)blk + (nBlk * blkSize); pos += (blkSize & ~0)) {
        blockFunc(pos);
    }
}

void SystemHeap_RunBlockFunc1(void* blk, size_t nBlk, size_t blkSize, BlockFunc1 blockFunc) {
    uintptr_t pos = (uintptr_t)blk;

    for (; pos < (uintptr_t)blk + (nBlk * blkSize); pos += (blkSize & ~0)) {
        blockFunc(pos, 2);
    }
}

void* SystemHeap_RunBlockFunc8(void* blk, size_t nBlk, size_t blkSize, BlockFunc8 blockFunc) {
    if (blk == NULL) {
        blk = SystemHeap_Malloc(nBlk * blkSize);
    }

    if ((blk != NULL) && (blockFunc != NULL)) {
        uintptr_t pos = (uintptr_t)blk;

        for (; pos < (uintptr_t)blk + (nBlk * blkSize); pos += (blkSize & ~0)) {
            blockFunc(pos, 0, 0, 0, 0, 0, 0, 0, 0);
        }
    }

    return blk;
}

void SystemHeap_RunBlockFunc1Reverse(void* blk, size_t nBlk, size_t blkSize, BlockFunc1 blockFunc, s32 shouldFree) {
    uintptr_t pos;
    uintptr_t start;
    size_t maskedBlkSize;

    if (blk == NULL) {
        return;
    }

    if (blockFunc != NULL) {
        start = (uintptr_t)blk;
        maskedBlkSize = (blkSize & ~0);
        pos = (uintptr_t)start + (nBlk * blkSize);

        while (pos > start) {
            pos -= maskedBlkSize;
            blockFunc(pos, 2);
        }
    }

    if (shouldFree) {
        SystemHeap_Free(blk);
    }
}

void SystemHeap_RunInits(void) {
    InitFunc* initFunc = (InitFunc*)&sInitFuncs;
    u32 nextOffset = initFunc->nextOffset;
    InitFunc* prev = NULL;

    while (nextOffset != 0) {
        initFunc = (InitFunc*)((uintptr_t)initFunc + nextOffset);

        if (initFunc->func != NULL) {
            (*initFunc->func)();
        }

        nextOffset = initFunc->nextOffset;
        initFunc->nextOffset = (uintptr_t)prev;
        prev = initFunc;
    }

    sInitFuncs = prev;
}

void SystemHeap_Init(void* start, size_t size) {
    SystemArena_Init(start, size);
    SystemHeap_RunInits();
}

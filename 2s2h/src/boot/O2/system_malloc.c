#include "global.h"
#include "os_malloc.h"
#include <string.h>

Arena gSystemArena;

void* SystemArena_Malloc(size_t size) {
    return __osMalloc(&gSystemArena, size);
}

void* SystemArena_MallocR(size_t size) {
    return __osMallocR(&gSystemArena, size);
}

void* SystemArena_Realloc(void* oldPtr, size_t newSize) {
    return __osRealloc(&gSystemArena, oldPtr, newSize);
}

void SystemArena_Free(void* ptr) {
    __osFree(&gSystemArena, ptr);
}

void* SystemArena_Calloc(size_t num, size_t size) {
    void* ptr;
    size_t totalSize = num * size;

    ptr = __osMalloc(&gSystemArena, totalSize);
    if (ptr != NULL) {
        memset(ptr, 0, totalSize);
    }
    return ptr;
}

void SystemArena_GetSizes(size_t* maxFreeBlock, size_t* bytesFree, size_t* bytesAllocated) {
    __osGetSizes(&gSystemArena, maxFreeBlock, bytesFree, bytesAllocated);
}

u32 SystemArena_CheckArena(void) {
    return __osCheckArena(&gSystemArena);
}

void SystemArena_Init(void* start, size_t size) {
    __osMallocInit(&gSystemArena, start, size);
}

void SystemArena_Cleanup(void) {
    __osMallocCleanup(&gSystemArena);
}

u8 SystemArena_IsInitialized(void) {
    return __osMallocIsInitalized(&gSystemArena);
}

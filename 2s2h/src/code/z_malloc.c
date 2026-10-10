#include "z64malloc.h"

#include "os_malloc.h"
#include <string.h>

Arena sZeldaArena;

void* ZeldaArena_Malloc(size_t size) {
    void* ptr = __osMalloc(&sZeldaArena, size);

    return ptr;
}

void* ZeldaArena_MallocR(size_t size) {
    void* ptr = __osMallocR(&sZeldaArena, size);

    return ptr;
}

void* ZeldaArena_Realloc(void* ptr, size_t newSize) {
    ptr = __osRealloc(&sZeldaArena, ptr, newSize);
    return ptr;
}

void ZeldaArena_Free(void* ptr) {
    __osFree(&sZeldaArena, ptr);
}

void* ZeldaArena_Calloc(size_t num, size_t size) {
    void* ptr;
    size_t totalSize = num * size;

    ptr = __osMalloc(&sZeldaArena, totalSize);
    if (ptr != NULL) {
        memset(ptr, 0, totalSize);
    }

    return ptr;
}

void ZeldaArena_GetSizes(size_t* outMaxFree, size_t* outFree, size_t* outAlloc) {
    __osGetSizes(&sZeldaArena, outMaxFree, outFree, outAlloc);
}

s32 ZeldaArena_Check(void) {
    return __osCheckArena(&sZeldaArena);
}

void ZeldaArena_Init(void* start, size_t size) {
    __osMallocInit(&sZeldaArena, start, size);
}

void ZeldaArena_Cleanup(void) {
    __osMallocCleanup(&sZeldaArena);
}

u8 ZeldaArena_IsInitialized(void) {
    return __osMallocIsInitalized(&sZeldaArena);
}

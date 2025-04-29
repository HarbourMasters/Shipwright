#include "global.h"
#include <string.h>

#define LOG_SEVERITY_NOLOG 0
#define LOG_SEVERITY_ERROR 2
#define LOG_SEVERITY_VERBOSE 3

s32 gZeldaArenaLogSeverity = LOG_SEVERITY_ERROR;
Arena sZeldaArena;

void ZeldaArena_CheckPointer(void* ptr, size_t size, const char* name, const char* action) {
    if (ptr == NULL) {
        if (gZeldaArenaLogSeverity >= LOG_SEVERITY_ERROR) {
            // "%s: %u bytes %s failed\n"
            osSyncPrintf("%s: %u バイトの%sに失敗しました\n", name, size, action);
            __osDisplayArena(&sZeldaArena);
        }
    } else if (gZeldaArenaLogSeverity >= LOG_SEVERITY_VERBOSE) {
        // "%s: %u bytes %s succeeded\n"
        osSyncPrintf("%s: %u バイトの%sに成功しました\n", name, size, action);
    }
}

void* ZeldaArena_Malloc(size_t size) {
    void* ptr = __osMalloc(&sZeldaArena, size);

    ZeldaArena_CheckPointer(ptr, size, "zelda_malloc", "確保"); // "Secure"
    return ptr;
}

void* ZeldaArena_MallocDebug(size_t size, const char* file, s32 line) {
    void* ptr = __osMallocDebug(&sZeldaArena, size, file, line);

    ZeldaArena_CheckPointer(ptr, size, "zelda_malloc_DEBUG", "確保"); // "Secure"
    return ptr;
}

void* ZeldaArena_MallocR(size_t size) {
    void* ptr = __osMallocR(&sZeldaArena, size);

    ZeldaArena_CheckPointer(ptr, size, "zelda_malloc_r", "確保"); // "Secure"
    return ptr;
}

void* ZeldaArena_MallocRDebug(size_t size, const char* file, s32 line) {
    void* ptr = __osMallocRDebug(&sZeldaArena, size, file, line);

    ZeldaArena_CheckPointer(ptr, size, "zelda_malloc_r_DEBUG", "確保"); // "Secure"
    return ptr;
}

void* ZeldaArena_Realloc(void* ptr, size_t newSize) {
    ptr = __osRealloc(&sZeldaArena, ptr, newSize);
    ZeldaArena_CheckPointer(ptr, newSize, "zelda_realloc", "再確保"); // "Re-securing"
    return ptr;
}

void* ZeldaArena_ReallocDebug(void* ptr, size_t newSize, const char* file, s32 line) {
    ptr = __osReallocDebug(&sZeldaArena, ptr, newSize, file, line);
    ZeldaArena_CheckPointer(ptr, newSize, "zelda_realloc_DEBUG", "再確保"); // "Re-securing"
    return ptr;
}

void ZeldaArena_Free(void* ptr) {
    __osFree(&sZeldaArena, ptr);
}

void ZeldaArena_FreeDebug(void* ptr, const char* file, s32 line) {
    __osFreeDebug(&sZeldaArena, ptr, file, line);
}

void* ZeldaArena_Calloc(size_t num, size_t size) {
    void* ret;
    size_t n = num * size;

    ret = __osMalloc(&sZeldaArena, n);
    if (ret != NULL) {
        memset(ret, 0, n);
    }

    ZeldaArena_CheckPointer(ret, n, "zelda_calloc", "確保");
    return ret;
}

void ZeldaArena_Display() {
    osSyncPrintf("ゼルダヒープ表示\n"); // "Zelda heap display"
    __osDisplayArena(&sZeldaArena);
}

void ZeldaArena_GetSizes(u32* outMaxFree, u32* outFree, u32* outAlloc) {
    ArenaImpl_GetSizes(&sZeldaArena, outMaxFree, outFree, outAlloc);
}

void ZeldaArena_Check() {
    __osCheckArena(&sZeldaArena);
}

void ZeldaArena_Init(void* start, size_t size) {
    gZeldaArenaLogSeverity = LOG_SEVERITY_NOLOG;
    __osMallocInit(&sZeldaArena, start, size);
}

void ZeldaArena_Cleanup() {
    gZeldaArenaLogSeverity = LOG_SEVERITY_NOLOG;
    __osMallocCleanup(&sZeldaArena);
}

u8 ZeldaArena_IsInitalized() {
    return __osMallocIsInitialized(&sZeldaArena);
}

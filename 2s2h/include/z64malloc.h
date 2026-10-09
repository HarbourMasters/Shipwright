#ifndef Z64MALLOC_H
#define Z64MALLOC_H

#include "ultra64.h"

void* ZeldaArena_Malloc(size_t size);
void* ZeldaArena_MallocR(size_t size);
void* ZeldaArena_Realloc(void* ptr, size_t newSize);
void ZeldaArena_Free(void* ptr);
void* ZeldaArena_Calloc(size_t num, size_t size);
void ZeldaArena_GetSizes(size_t* outMaxFree, size_t* outFree, size_t* outAlloc);
s32 ZeldaArena_Check(void);
void ZeldaArena_Init(void* start, size_t size);
void ZeldaArena_Cleanup(void);
u8 ZeldaArena_IsInitialized(void);

#endif

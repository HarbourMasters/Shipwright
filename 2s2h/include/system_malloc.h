#ifndef SYSTEM_MALLOC_H
#define SYSTEM_MALLOC_H

#include "PR/ultratypes.h"
#include "os_malloc.h"

void* SystemArena_Malloc(size_t size);
void* SystemArena_MallocR(size_t size);
void* SystemArena_Realloc(void* oldPtr, size_t newSize);
void SystemArena_Free(void* ptr);
void* SystemArena_Calloc(size_t num, size_t size);
void SystemArena_GetSizes(size_t* maxFreeBlock, size_t* bytesFree, size_t* bytesAllocated);
u32 SystemArena_CheckArena(void);
void SystemArena_Init(void* start, size_t size);
void SystemArena_Cleanup(void);
u8 SystemArena_IsInitialized(void);

extern Arena gSystemArena;

#endif

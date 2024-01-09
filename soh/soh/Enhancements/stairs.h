#ifndef STAIRS_H
#define STAIRS_H

#include "global.h"


#define STAIRS_ACTOROVL_ABSOLUTE_SPACE_SIZE 0x27A0
#define STAIRS_ACTOROVL_ALLOC_ABSOLUTE (1 << 0)
#define STAIRS_ACTOROVL_ALLOC_PERSISTENT (1 << 1)

// This value is based on the ZeldaArena size created on play init with default GameAlloc size
// #define STAIRS_ALLOC_SIZE 560064
// #define STAIRS_ALLOC_SIZE 0x88BC0

#ifdef __cplusplus
extern "C" {
#endif

u16 Stairs_GetAllocType(u16 id);
s32 Stairs_RegisterOverlay(u16 id);
s32 Stairs_GetOverlayRegistered(u16 id);
s32 Stairs_UnregisterOverlay(u16 id);
s32 Stairs_RegisterAbsoluteSpace();
s32 Stairs_GetAbsoluteSpace();
s32 Stairs_UnregisterAbsoluteSpace();
size_t Stairs_GetOverlaySize(u16 id);

size_t Stairs_GetSize();
s32 Stairs_DecreaseSize(size_t size);

void* StairsArena_Malloc(size_t size);
void* StairsArena_MallocR(size_t size);
void* StairsArena_Realloc(void* ptr, size_t newSize);
void StairsArena_Free(void* ptr);
void* StairsArena_Calloc(size_t num, size_t size);
void StairsArena_Display();
void StairsArena_GetSizes(u32* outMaxFree, u32* outFree, u32* outAlloc);
void StairsArena_Check();
void StairsArena_Init(void* start, size_t size);
void StairsArena_Cleanup();
u8 StairsArena_IsInitalized();

void* StairsArena_MallocGeneral(size_t size, uintptr_t id);
void* StairsArena_MallocROverlay(size_t size, u16 id);
void* StairsArena_MallocOverlay(size_t size, u16 id);
void* StairsArena_MallocRGeneral(size_t size, uintptr_t id);
void* StairsArena_MallocRAbsolute(size_t size);
void StairsArena_FreeGeneral(uintptr_t id);
void StairsArena_FreeMallocROverlay(u16 id);
void StairsArena_FreeOverlay(u16 id);
void StairsArena_FreeMallocR(u16 id);
void StairsArena_FreeAbsolute();

#ifdef __cplusplus
}
#endif

#endif //STAIRS_H
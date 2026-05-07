#pragma once

#include <libultraship/libultraship.h>

#ifdef __cplusplus
extern "C" {
#endif


// Sentinel value for null offsets (no valid node can live at 0xFFFFFFFF in a buffer that's only ~245KB).
#define SHADOW_NULL 0xFFFFFFFF

// Matches N64 retail ArenaNode: 0x10 bookkeeping + 0x20 debug fields.
#define SHADOW_NODE_SIZE 0x30

typedef struct shadow_arena
{
    u8* buffer;
    u32 head; // Offset to first node
    u32 bufferSize;
} ShadowArena;

// Allocate backing buffer and initialize with a single fee node. Size should be the N64 ZeldaArena size (0x3D550).
void ShadowArena_Init(ShadowArena* arena, u32 size);

// Free the backing buffer and zero the struct.
void ShadowArena_Destroy(ShadowArena* arena);

// First-fit forward allocation. Returns offset to data area, or SHADOW_NULL on failure. Matches N64 __osMalloc.
u32 ShadowArena_Malloc(ShadowArena* arena, u32 size);

// First-fit backward allocation. Returns offset to data area, or SHADOW_NULL on failure. Matches N64 __osMallocR.
u32 ShadowArena_MallocR(ShadowArena* arena, u32 size);

// Free a shadow allocation by data offset. Coalesces adjacent free blocks. Matches N64 __osFree.
void ShadowArena_Free(ShadowArena* arena, u32 dataOffset);

// Query arena statistics.
void ShadowArena_GetSizes(ShadowArena* arena, u32* outMaxFree, u32* outFree, u32* outAlloc);

// Get the head node offset for external traversal (e.g., heap viewer).
u32 ShadowArena_GetHead(ShadowArena* arena);

#ifdef __cplusplus
}
#endif

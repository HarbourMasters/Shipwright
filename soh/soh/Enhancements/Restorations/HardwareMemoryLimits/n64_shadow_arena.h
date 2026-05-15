#pragma once

#include <libultraship/libultraship.h>

#ifdef __cplusplus
extern "C" {
#endif

// Sentinel value for null offsets (no valid node can live at 0xFFFFFFFF in a buffer that's only ~245KB).
#define SHADOW_NULL 0xFFFFFFFF

// Default node size for the shadow arena.  Overridden at init from OTR data.
//   Retail N64 (no debug fields):  0x10
//   GC Debug   (debug fields):     0x30
#define SHADOW_NODE_SIZE_RETAIL 0x10
#define SHADOW_NODE_SIZE_DEBUG  0x30

typedef struct ShadowArena {
    u8* buffer;
    u32 head; // Offset to first node
    u32 bufferSize;
    u32 nodeSize; // Per-version ArenaNode size (set at init from OTR data)
} ShadowArena;

// Allocate backing buffer and initialize with a single free node.  nodeSize is the N64 ArenaNode size for this ROM
// version (0x10 for retail, 0x30 for debug).
void ShadowArena_Init(ShadowArena* arena, u32 size, u32 nodeSize);

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

// Query a node's info by offset.  Returns 1 on success, 0 if invalid.  Used by the heap viewer to walk the shadow
// without exposing internals.
s32 ShadowArena_GetNodeInfo(ShadowArena* arena, u32 offset, s32* outIsFree, u32* outSize, u32* outNext);

// Get the total buffer size.
u32 ShadowArena_GetBufferSize(ShadowArena* arena);

#ifdef __cplusplus
}
#endif
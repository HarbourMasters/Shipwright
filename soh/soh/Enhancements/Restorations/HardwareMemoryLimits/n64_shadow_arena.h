#pragma once

#include <stdint.h>

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
    uint8_t* buffer;
    uint32_t head; // Offset to first node
    uint32_t bufferSize;
    uint32_t nodeSize; // Per-version ArenaNode size (set at init from OTR data)
} ShadowArena;

// Allocate backing buffer and initialize with a single free node.  nodeSize is the N64 ArenaNode size for this ROM
// version (0x10 for retail, 0x30 for debug).
void ShadowArena_Init(ShadowArena* arena, uint32_t size, uint32_t nodeSize);

// Free the backing buffer and zero the struct.
void ShadowArena_Destroy(ShadowArena* arena);

// First-fit forward allocation. Returns offset to data area, or SHADOW_NULL on failure. Matches N64 __osMalloc.
uint32_t ShadowArena_Malloc(ShadowArena* arena, uint32_t size);

// First-fit backward allocation. Returns offset to data area, or SHADOW_NULL on failure. Matches N64 __osMallocR.
uint32_t ShadowArena_MallocR(ShadowArena* arena, uint32_t size);

// Free a shadow allocation by data offset. Coalesces adjacent free blocks. Matches N64 __osFree.
void ShadowArena_Free(ShadowArena* arena, uint32_t dataOffset);

// Query arena statistics.
void ShadowArena_GetSizes(ShadowArena* arena, uint32_t* outMaxFree, uint32_t* outFree, uint32_t* outAlloc);

// Get the head node offset for external traversal (e.g., heap viewer).
uint32_t ShadowArena_GetHead(ShadowArena* arena);

// Query a node's info by offset.  Returns true on success, false if invalid.  Used by the heap viewer to walk the
// shadow without exposing internals.
int32_t ShadowArena_GetNodeInfo(ShadowArena* arena, uint32_t offset, int32_t* outIsFree, uint32_t* outSize,
                                uint32_t* outNext);

// Get the total buffer size.
uint32_t ShadowArena_GetBufferSize(ShadowArena* arena);

#ifdef __cplusplus
}
#endif
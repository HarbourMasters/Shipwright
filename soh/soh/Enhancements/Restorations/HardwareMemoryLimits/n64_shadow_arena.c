#include "n64_shadow_arena.h"

#include <stdlib.h>
#include <string.h>

// --------------------------------------------------------------------------------------------------------------------
// Internal node layout (not exposed -- heap viewer uses offset + size queries, not direct struct access).
//
// Fixed header fields (0x10 bytes, identical on retail and debug):
//  0x00        s16         magic
//  0x02        s16         isFree
//  0x04        u32         size    (Payload bytes, excluding node header)
//  0x08        u32         next    (Offset into buffer, SHADOW_NULL = end)
//  0x0C        u32         prev    (Offset into buffer, SHADOW_NULL = head)
//
// On debug builds, an additional 0x20 bytes of debug fields follow (filename, line, threadId, arena, time).
// The total node spacing is arena->nodeSize: 0x10 for retail, 0x30 for debug.
// --------------------------------------------------------------------------------------------------------------------

typedef struct ShadowNode
{
    s16 magic;
    s16 isFree;
    u32 size;
    u32 next;
    u32 prev;
} ShadowNode; // 0x10

static_assert(sizeof(ShadowNode) == 0x10, "ShadowNode header must be exactly 0x10 bytes");

#define NODE_MAGIC 0x7373
#define ALIGN16(x) (((x) + 0xF) & ~0xF)

// --------------------------------------------------------------------------------------------------------------------
// Offset helpers
// --------------------------------------------------------------------------------------------------------------------

static ShadowNode* NodeAt(ShadowArena* arena, u32 offset)
{
    return (ShadowNode*)(arena->buffer + offset);
}

static s32 NodeIsValid(ShadowArena* arena, u32 offset)
{
    if (offset == SHADOW_NULL)
    {
        return 0;
    }

    if (offset + arena->nodeSize > arena->bufferSize)
    {
        return 0;
    }

    return NodeAt(arena, offset)->magic == NODE_MAGIC;
}

static u32 NodeGetNext(ShadowArena* arena, u32 offset)
{
    const ShadowNode* node = NodeAt(arena, offset);
    if (node->next != SHADOW_NULL && NodeIsValid(arena, node->next))
    {
        return node->next;
    }

    return SHADOW_NULL;
}

static u32 NodeGetPrev(ShadowArena* arena, u32 offset)
{
    const ShadowNode* node = NodeAt(arena, offset);
    if (node->prev != SHADOW_NULL && NodeIsValid(arena, node->prev))
    {
        return node->prev;
    }

    return SHADOW_NULL;
}

// --------------------------------------------------------------------------------------------------------------------
// Init / Destroy
// --------------------------------------------------------------------------------------------------------------------

void ShadowArena_Init(ShadowArena* arena, u32 size, u32 nodeSize)
{
    // Match N64's alignment: Round start up to 16, round size down to 16.  Since we control the buffer, start is
    // effectively offset 0 after alignment.  We just ensure the usable size is 16-byte aligned.
    const u32 alignedSize = size & ~0xF;

    arena->buffer = (u8*)malloc(alignedSize);
    if (!arena->buffer)
    {
        memset(arena, 0, sizeof(ShadowArena));
        return;
    }

    memset(arena->buffer, 0, alignedSize);
    arena->bufferSize = alignedSize;
    arena->nodeSize = nodeSize;

    // Single free node spanning the entire buffer minus one header.
    ShadowNode* first = NodeAt(arena, 0);
    first->magic = NODE_MAGIC;
    first->isFree = 1;
    first->size = alignedSize - nodeSize;
    first->next = SHADOW_NULL;
    first->prev = SHADOW_NULL;

    arena->head = 0;
}

void ShadowArena_Destroy(ShadowArena* arena)
{
    if (arena->buffer)
    {
        free(arena->buffer);
    }

    memset(arena, 0, sizeof(ShadowArena));
}

// --------------------------------------------------------------------------------------------------------------------
// Malloc: First-fit forward (matches N64 __osMalloc)
// --------------------------------------------------------------------------------------------------------------------

u32 ShadowArena_Malloc(ShadowArena* arena, u32 size)
{
    u32 iterOff = 0;
    ShadowNode* iter = NULL;
    u32 blockSize = 0;

    size = ALIGN16(size);
    blockSize = size + arena->nodeSize;

    iterOff = arena->head;
    while (iterOff != SHADOW_NULL)
    {
        iter = NodeAt(arena, iterOff);
        if (iter->isFree && iter->size >= size)
        {
            // Split if remainder can hold a new node + payload.
            if (blockSize < iter->size)
            {
                const u32 newOff = iterOff + blockSize;
                ShadowNode* newNode = NodeAt(arena, newOff);

                newNode->magic = NODE_MAGIC;
                newNode->isFree = 1;
                newNode->size = iter->size - blockSize;
                newNode->next = iter->next;
                newNode->prev = iterOff;

                if (newNode->next != SHADOW_NULL)
                {
                    NodeAt(arena, newNode->next)->prev = newOff;
                }

                iter->next = newOff;
                iter->size = size;
            }

            iter->isFree = 0;

            // Return offset to data area (past the node header).
            return iterOff + arena->nodeSize;
        }

        iterOff = NodeGetNext(arena, iterOff);
    }

    return SHADOW_NULL;
}

// --------------------------------------------------------------------------------------------------------------------
// MallocR: First-fit backward (matches N64 __osMallocR)
// --------------------------------------------------------------------------------------------------------------------

u32 ShadowArena_MallocR(ShadowArena* arena, u32 size)
{
    u32 iterOff = 0;
    u32 nextOff = 0;
    ShadowNode* iter = NULL;
    u32 blockSize = 0;

    size = ALIGN16(size);

    // Walk to tail.
    iterOff = arena->head;
    nextOff = NodeGetNext(arena, iterOff);
    while (nextOff != SHADOW_NULL)
    {
        iterOff = nextOff;
        nextOff = NodeGetNext(arena, nextOff);
    }

    // Walk backward looking for a fit.
    while (iterOff != SHADOW_NULL)
    {
        iter = NodeAt(arena, iterOff);
        if (iter->isFree && iter->size >= size)
        {
            blockSize = size + arena->nodeSize;

            // Split: Carve the allocation from the TOP of this free block.
            if (blockSize < iter->size)
            {
                const u32 newOff = iterOff + (iter->size - size);
                ShadowNode* newNode = NodeAt(arena, newOff);

                newNode->magic = NODE_MAGIC;
                newNode->isFree = 0;
                newNode->size = size;
                newNode->next = iter->next;
                newNode->prev = iterOff;

                if (newNode->next != SHADOW_NULL)
                {
                    NodeAt(arena, newNode->next)->prev = newOff;
                }

                iter->next = newOff;
                iter->size -= blockSize;

                return newOff + arena->nodeSize;
            }

            // No split, use the whole block.
            iter->isFree = 0;
            return iterOff + arena->nodeSize;
        }

        iterOff = NodeGetPrev(arena, iterOff);
    }

    return SHADOW_NULL;
}

// --------------------------------------------------------------------------------------------------------------------
// Free: Adjacent block coalescing (matches N64 _osFree)
// --------------------------------------------------------------------------------------------------------------------

void ShadowArena_Free(ShadowArena* arena, u32 dataOffset)
{
    u32 nodeOff = 0;
    ShadowNode* node = NULL;
    u32 nextOff = 0;
    u32 prevOff = 0;

    if (dataOffset == SHADOW_NULL)
    {
        return;
    }

    nodeOff = dataOffset - arena->nodeSize;
    node = NodeAt(arena, nodeOff);

    if (node->magic != NODE_MAGIC)
    {
        return;
    }

    if (node->isFree)
    {
        return;
    }

    node->isFree = 1;

    // Forward coalesce: Merge with next if it's free.
    nextOff = node->next;
    if (nextOff != SHADOW_NULL)
    {
        const ShadowNode* next = NodeAt(arena, nextOff);
        if (next->isFree)
        {
            if (next->next != SHADOW_NULL)
            {
                NodeAt(arena, next->next)->prev = nodeOff;
            }

            node->size += next->size + arena->nodeSize;
            node->next = next->next;
        }
    }

    // Backward coalesce: Merge into prev if it's free.
    prevOff = node->prev;
    if (prevOff != SHADOW_NULL)
    {
        ShadowNode* prev = NodeAt(arena, prevOff);
        if (prev->isFree)
        {
            prev->size += node->size + arena->nodeSize;
            prev->next = node->next;

            if (node->next != SHADOW_NULL)
            {
                NodeAt(arena, node->next)->prev = prevOff;
            }
        }
    }
}

// --------------------------------------------------------------------------------------------------------------------
// Query
// --------------------------------------------------------------------------------------------------------------------

void ShadowArena_GetSizes(ShadowArena* arena, u32* outMaxFree, u32* outFree, u32* outAlloc)
{
    u32 iterOff = 0;

    *outMaxFree = 0;
    *outFree = 0;
    *outAlloc = 0;

    iterOff = arena->head;
    while (iterOff != SHADOW_NULL)
    {
        const ShadowNode* node = NodeAt(arena, iterOff);
        if (node->isFree)
        {
            *outFree += node->size;
            if (node->size > *outMaxFree)
            {
                *outMaxFree = node->size;
            }
        }
        else
        {
            *outAlloc += node->size;
        }

        iterOff = NodeGetNext(arena, iterOff);
    }
}

u32 ShadowArena_GetHead(ShadowArena* arena)
{
    return arena->head;
}

s32 ShadowArena_GetNodeInfo(ShadowArena* arena, u32 offset, s32* outIsFree, u32* outSize, u32* outNext)
{
    if (offset == SHADOW_NULL || offset + arena->nodeSize > arena->bufferSize)
    {
        return 0;
    }

    const ShadowNode* node = NodeAt(arena, offset);
    if (node->magic != NODE_MAGIC)
    {
        return 0;
    }

    *outIsFree = node->isFree;
    *outSize = node->size;
    *outNext = node->next != SHADOW_NULL && NodeIsValid(arena, node->next) ? node->next : SHADOW_NULL;
    return 1;
}

u32 ShadowArena_GetBufferSize(ShadowArena* arena)
{
    return arena->bufferSize;
}
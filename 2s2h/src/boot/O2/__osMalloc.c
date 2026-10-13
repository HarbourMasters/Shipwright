#include "os_malloc.h"
#include "libc/stdbool.h"
#include "libc/stdint.h"
#include "macros.h"
#include "functions.h"
#include <string.h>

#define FILL_ALLOCBLOCK (1 << 0)
#define FILL_FREEBLOCK (1 << 1)
#define CHECK_FREE_BLOCK (1 << 2)

#define NODE_MAGIC (0x7373)

#define BLOCK_UNINIT_MAGIC (0xAB)
#define BLOCK_UNINIT_MAGIC_32 (0xABABABAB)
#define BLOCK_ALLOC_MAGIC (0xCD)
#define BLOCK_ALLOC_MAGIC_32 (0xCDCDCDCD)
#define BLOCK_FREE_MAGIC (0xEF)
#define BLOCK_FREE_MAGIC_32 (0xEFEFEFEF)

OSMesg sArenaLockMsg[1];

void __osMallocAddHeap(Arena* arena, void* heap, size_t size);

void ArenaImpl_LockInit(Arena* arena) {
    osCreateMesgQueue(&arena->lock, sArenaLockMsg, ARRAY_COUNT(sArenaLockMsg));
}

void ArenaImpl_Lock(Arena* arena) {
    // osSendMesg(&arena->lock, NULL, OS_MESG_BLOCK);
}

void ArenaImpl_Unlock(Arena* arena) {
    // osRecvMesg(&arena->lock, NULL, OS_MESG_BLOCK);
}

ArenaNode* ArenaImpl_GetLastBlock(Arena* arena) {
    ArenaNode* last;
    ArenaNode* iter;

    last = arena->head;

    if (last != NULL) {
        iter = last->next;
        while (iter != NULL) {
            last = iter;
            iter = iter->next;
        }
    }
    return last;
}

/**
 * Initializes \p arena to manage the memory region \p heap.
 *
 * @param arena  The Arena to initialize.
 * @param heap   The memory region to use as heap space.
 * @param size   The size of the heap.
 */
void __osMallocInit(Arena* arena, void* heap, size_t size) {
    memset(arena, 0, sizeof(Arena));

    ArenaImpl_LockInit(arena);

    __osMallocAddHeap(arena, heap, size);
    arena->isInit = true;
}

// Original name: __osMallocAddBlock
void __osMallocAddHeap(Arena* arena, void* heap, size_t size) {
    ptrdiff_t diff;
    s32 alignedSize;
    ArenaNode* firstNode;
    ArenaNode* lastNode;

    if (heap == NULL) {
        return;
    }

    firstNode = (ArenaNode*)ALIGN16((uintptr_t)heap);
    diff = (uintptr_t)firstNode - (uintptr_t)heap;
    alignedSize = ((s32)size - diff) & ~0xF;

    // If the size of the heap is smaller than sizeof(ArenaNode), then the initialization will silently fail
    if (alignedSize > (s32)sizeof(ArenaNode)) {
        firstNode->next = NULL;
        firstNode->prev = NULL;
        firstNode->size = alignedSize - sizeof(ArenaNode);
        firstNode->isFree = true;
        firstNode->magic = NODE_MAGIC;

        ArenaImpl_Lock(arena);

        lastNode = ArenaImpl_GetLastBlock(arena);

        // Checks if there's already a block
        if (lastNode == NULL) {
            arena->head = firstNode;
            arena->start = heap;
        } else {
            // Chain the existing block with the new one
            firstNode->prev = lastNode;
            lastNode->next = firstNode;
        }

        ArenaImpl_Unlock(arena);
    }
}

/**
 * Clears the whole \p arena, invalidating every allocated pointer to it.
 *
 * @param arena  The Arena to clear.
 */
void __osMallocCleanup(Arena* arena) {
    memset(arena, 0, sizeof(Arena));
}

/**
 * Returns whether or not the \p arena has been initialized.
 *
 * @param arena  The Arena to check.
 * @return u8    `true` if the \p arena has been initialized. `false` otherwise.
 */
u8 __osMallocIsInitalized(Arena* arena) {
    return arena->isInit;
}

/**
 * Allocates at least \p size bytes of memory using the given \p arena.
 * The block of memory will be allocated at the start of the first sufficiently large free block.
 *
 *  - If there's not enough space in the given \p arena, this function will fail, returning `NULL`.
 *  - If \p size is zero, then an empty region of memory is returned.
 *
 * To avoid memory leaks, the returned pointer should be eventually deallocated using either `__osFree` or
 * `__osRealloc`.
 *
 * @param[in, out] arena  The specific Arena to be used for the allocation.
 * @param[in] size        The size in bytes that will be allocated.
 * @return void*          On success, the allocated area of the \p arena memory. Otherwise, `NULL`.
 */
void* __osMalloc(Arena* arena, size_t size) {
    ArenaNode* iter;
    ArenaNode* newNode;
    void* alloc = NULL;

    size = ALIGN16(size);

    ArenaImpl_Lock(arena);

    // Start iterating from the head of the arena.
    iter = arena->head;

    // Iterate over the arena looking for a big enough space of memory.
    while (iter != NULL) {
        if (iter->isFree && iter->size >= size) {
            size_t blockSize = ALIGN16(size) + sizeof(ArenaNode);

            // If the block is larger than the requested size, then split it and just use the required size of the
            // current block.
            if (blockSize < iter->size) {
                ArenaNode* next;

                newNode = (ArenaNode*)((uintptr_t)iter + blockSize);
                newNode->next = iter->next;
                newNode->prev = iter;
                newNode->size = iter->size - blockSize;
                newNode->isFree = true;
                newNode->magic = NODE_MAGIC;

                iter->next = newNode;
                iter->size = size;

                next = newNode->next;
                if (next != NULL) {
                    next->prev = newNode;
                }
            }

            iter->isFree = false;
            alloc = (void*)((uintptr_t)iter + sizeof(ArenaNode));
            break;
        }

        iter = iter->next;
    }

    ArenaImpl_Unlock(arena);

    return alloc;
}

/**
 * Allocates at least \p size bytes of memory using the given \p arena.
 * Unlike __osMalloc, the block of memory will be allocated from the end of the \p arena.
 *
 * - If there's not enough space in the given \p arena, this function will fail, returning `NULL`.
 * - If \p size is zero, then an empty region of memory is returned.
 *
 * To avoid memory leaks, the returned pointer should be eventually deallocated using `__osFree` or `__osRealloc`.
 *
 * @param[in, out] arena  The specific Arena to be used for the allocation.
 * @param[in] size        The size in bytes that will be allocated.
 * @return void*          On success, the allocated area of the \p arena memory. Otherwise, `NULL`.
 */
void* __osMallocR(Arena* arena, size_t size) {
    ArenaNode* iter;
    ArenaNode* newNode;
    size_t blockSize;
    void* alloc = NULL;

    size = ALIGN16(size);

    ArenaImpl_Lock(arena);

    // Start iterating from the last block of the arena.
    iter = ArenaImpl_GetLastBlock(arena);

    // Iterate in reverse the arena looking for a big enough space of memory.
    while (iter != NULL) {
        if (iter->isFree && iter->size >= size) {
            blockSize = ALIGN16(size) + sizeof(ArenaNode);

            // If the block is larger than the requested size, then split it and just use the required size of the
            // current block.
            if (blockSize < iter->size) {
                ArenaNode* next;

                newNode = (ArenaNode*)((uintptr_t)iter + (iter->size - size));
                newNode->next = iter->next;
                newNode->prev = iter;
                newNode->size = size;
                newNode->magic = NODE_MAGIC;

                iter->next = newNode;
                iter->size -= blockSize;

                next = newNode->next;
                if (next != NULL) {
                    next->prev = newNode;
                }
                iter = newNode;
            }

            iter->isFree = false;
            alloc = (void*)((uintptr_t)iter + sizeof(ArenaNode));
            break;
        }
        iter = iter->prev;
    }

    ArenaImpl_Unlock(arena);

    return alloc;
}

/**
 * Deallocates the pointer \p ptr previously allocated by `__osMalloc`, `__osMallocR` or `__osRealloc`.
 * If \p ptr is `NULL` or it has been already been freed, then this function does nothing.
 *
 * - The behaviour is undefined if \p ptr is not a memory region returned by one of the cited allocating
 * functions.
 * - The behaviour is undefined if \p ptr doesn't correspond to the given \p arena.
 * - Any access to the freed pointer is undefined behaviour.
 *
 * @param[in, out] arena  The specific Arena to be used for the allocation.
 * @param[in, out] ptr    The allocated memory block to deallocate.
 */
void __osFree(Arena* arena, void* ptr) {
    ArenaNode* node;
    ArenaNode* next;
    ArenaNode* prev;

    ArenaImpl_Lock(arena);

    node = (ArenaNode*)((uintptr_t)ptr - sizeof(ArenaNode));

    if ((ptr != NULL) && (node->magic == NODE_MAGIC) && !node->isFree) {
        next = node->next;
        prev = node->prev;
        node->isFree = true;

        // Checks if the next node is contiguous to the current node and if it isn't currently allocated. Then merge the
        // two nodes into one.
        if ((uintptr_t)next == (uintptr_t)node + sizeof(ArenaNode) + node->size && next->isFree) {
            ArenaNode* newNext = next->next;

            if (newNext != NULL) {
                newNext->prev = node;
            }

            node->size += next->size + sizeof(ArenaNode);

            node->next = newNext;
            next = newNext;
        }

        // Checks if the previous node is contiguous to the current node and if it isn't currently allocated. Then merge
        // the two nodes into one.
        if ((prev != NULL) && prev->isFree && ((uintptr_t)node == (uintptr_t)prev + sizeof(ArenaNode) + prev->size)) {
            if (next != NULL) {
                next->prev = prev;
            }

            prev->next = next;
            prev->size += node->size + sizeof(ArenaNode);
        }
    }

    ArenaImpl_Unlock(arena);
}

/**
 * Reallocates the pointer \p ptr.
 * \p ptr must be either a pointer previously allocated by `__osMalloc`, `__osMallocR` or `__osRealloc` and
 * not freed yet, or a `NULL` pointer.
 *
 * - If \p ptr is `NULL` a new pointer is allocated. See `__osMalloc` for more details.
 * - If \p newSize is 0, then the given pointer is freed and `NULL` is returned. See `__osFree` for more details.
 * - If \p newSize is bigger than the currently allocated allocated pointer, then the area of memory is expanded to a
 * size big enough to fit the requested size.
 *
 * - The behaviour is undefined if \p ptr is not a memory region returned by one of the cited allocating
 * functions.
 * - The behaviour is undefined if \p ptr doesn't correspond to the given \p arena.
 * - If the pointer is freed, then any access to the original freed pointer is undefined behaviour.
 *
 * @param[in, out] arena  The specific Arena to be used for the allocation.
 * @param[in, out] ptr    The allocated memory block to deallocate.
 * @param[in] newSize     The new requested size.
 * @return void*          On success, the pointer to the reallocated area of memory. On failure, `NULL` is returned,
 * and the original parameter \p ptr remains valid.
 */
void* __osRealloc(Arena* arena, void* ptr, size_t newSize) {
    ArenaImpl_Lock(arena);

    (void)"__osRealloc(%08x, %d)\n";

    if (ptr == NULL) {
        // if the `ptr` is NULL, then allocate a new pointer with the specified size
        // if newSize is 0, then __osMalloc would return a NULL pointer
        ptr = __osMalloc(arena, newSize);
    } else if (newSize == 0) {
        // if the requested size is zero, then free the pointer
        __osFree(arena, ptr);
        ptr = NULL;
    } else {
        size_t diff;
        void* newPtr;
        // Gets the start of the ArenaNode pointer embedded
        ArenaNode* node = (void*)((uintptr_t)ptr - sizeof(ArenaNode));

        newSize = ALIGN16(newSize);

        // Only reallocate the memory if the new size isn't smaller than the actual node size
        if ((newSize != node->size) && (node->size < newSize)) {
            ArenaNode* next = node->next;

            diff = newSize - node->size;
            // Checks if the next node is contiguous to the current allocated node and it has enough space to fit the
            // new requested size
            if (((uintptr_t)next == (uintptr_t)node + node->size + sizeof(ArenaNode)) && (next->isFree) &&
                (next->size >= diff)) {
                ArenaNode* next2 = next->next;

                next->size = (next->size - diff);
                if (next2 != NULL) {
                    // Update the previous element of the linked list
                    next2->prev = (void*)((uintptr_t)next + diff);
                }

                next2 = (void*)((uintptr_t)next + diff);
                node->next = next2;
                node->size = newSize;
                __osMemcpy(next2, next, sizeof(ArenaNode));
            } else {
                // Create a new pointer and manually copy the data from the old pointer to the new one
                newPtr = __osMalloc(arena, newSize);
                if (newPtr != NULL) {
                    memcpy(ptr, newPtr, node->size);
                    __osFree(arena, ptr);
                }
                ptr = newPtr;
            }
        }
    }

    ArenaImpl_Unlock(arena);

    return ptr;
}

/**
 * Gets the size of the largest free block, the total free space and the total allocated space.
 *
 * @param[in, out] arena   The Arena which will be used to get the values from.
 * @param[out] outMaxFree  The size of the largest free block.
 * @param[out] outFree     The total free space.
 * @param[out] outAlloc    The total allocated space.
 */
void __osGetSizes(Arena* arena, size_t* outMaxFree, size_t* outFree, size_t* outAlloc) {
    ArenaNode* iter;

    ArenaImpl_Lock(arena);

    *outMaxFree = 0;
    *outFree = 0;
    *outAlloc = 0;

    iter = arena->head;
    while (iter != NULL) {
        if (iter->isFree) {
            *outFree += iter->size;
            if (*outMaxFree < iter->size) {
                *outMaxFree = iter->size;
            }
        } else {
            *outAlloc += iter->size;
        }

        iter = iter->next;
    }

    ArenaImpl_Unlock(arena);
}

/**
 * Checks the validity of every node of the \p arena.
 *
 * @param arena  The Arena to check.
 * @return s32   0 if every pointer is valid. 1 otherwise.
 */
s32 __osCheckArena(Arena* arena) {
    ArenaNode* iter;
    s32 err = 0;

    ArenaImpl_Lock(arena);

    // "Checking the contents of the arena..."
    (void)"アリーナの内容をチェックしています．．． (%08x)\n";

    for (iter = arena->head; iter != NULL; iter = iter->next) {
        if (iter->magic != NODE_MAGIC) {
            // "Oops!!"
            (void)"おおっと！！ (%08x %08x)\n";

            err = 1;
            break;
        }
    }

    // "The arena still looks good"
    (void)"アリーナはまだ、いけそうです\n";

    ArenaImpl_Unlock(arena);

    return err;
}

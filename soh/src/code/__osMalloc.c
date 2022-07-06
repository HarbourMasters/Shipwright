#include "global.h"
#include "vt.h"

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

OSMesg sArenaLockMsg;
u32 __osMalloc_FreeBlockTest_Enable;

u32 ArenaImpl_GetFillAllocBlock(Arena* arena) {
    return (arena->flag & FILL_ALLOCBLOCK) != 0;
}
u32 ArenaImpl_GetFillFreeBlock(Arena* arena) {
    return (arena->flag & FILL_FREEBLOCK) != 0;
}
u32 ArenaImpl_GetCheckFreeBlock(Arena* arena) {
    return (arena->flag & CHECK_FREE_BLOCK) != 0;
}

void ArenaImpl_SetFillAllocBlock(Arena* arena) {
    arena->flag |= FILL_ALLOCBLOCK;
}
void ArenaImpl_SetFillFreeBlock(Arena* arena) {
    arena->flag |= FILL_FREEBLOCK;
}
void ArenaImpl_SetCheckFreeBlock(Arena* arena) {
    arena->flag |= CHECK_FREE_BLOCK;
}

void ArenaImpl_UnsetFillAllocBlock(Arena* arena) {
    arena->flag &= ~FILL_ALLOCBLOCK;
}
void ArenaImpl_UnsetFillFreeBlock(Arena* arena) {
    arena->flag &= ~FILL_FREEBLOCK;
}
void ArenaImpl_UnsetCheckFreeBlock(Arena* arena) {
    arena->flag &= ~CHECK_FREE_BLOCK;
}

#if 0
void ArenaImpl_SetDebugInfo(ArenaNode* node, const char* file, s32 line, Arena* arena) {
    node->filename = file;
    node->line = line;
    node->threadId = osGetThreadId(NULL);
    node->arena = arena;
    node->time = osGetTime();
}
#endif
void ArenaImpl_LockInit(Arena* arena) {
    osCreateMesgQueue(&arena->lock, &sArenaLockMsg, 1);
}

void ArenaImpl_Lock(Arena* arena) {
    osSendMesgPtr(&arena->lock, NULL, OS_MESG_BLOCK);
}

void ArenaImpl_Unlock(Arena* arena) {
    osRecvMesg(&arena->lock, NULL, OS_MESG_BLOCK);
}

ArenaNode* ArenaImpl_GetNextBlock(ArenaNode* node) {
    ArenaNode* next = node->next;

    if (next != NULL && (next == NULL || (next->magic != NODE_MAGIC))) {
        osSyncPrintf(VT_COL(RED, WHITE) "緊急事態！メモリリーク発見！ (block=%08x)\n" VT_RST, next);
        next = NULL;
        node->next = NULL;
    }
    return next;
}

ArenaNode* ArenaImpl_GetPrevBlock(ArenaNode* node) {
    ArenaNode* prev = node->prev;

    if (prev != NULL && (prev == NULL || (prev->magic != NODE_MAGIC))) {
        osSyncPrintf(VT_COL(RED, WHITE) "緊急事態！メモリリーク発見！ (block=%08x)\n" VT_RST, prev);
        prev = NULL;
        node->prev = NULL;
    }
    return prev;
}

ArenaNode* ArenaImpl_GetLastBlock(Arena* arena) {
    ArenaNode* last = NULL;
    ArenaNode* iter;

    if (arena != NULL && arena->head != NULL && arena->head->magic == NODE_MAGIC) {
        iter = arena->head;
        while (iter != NULL) {
            last = iter;
            iter = ArenaImpl_GetNextBlock(iter);
        }
    }
    return last;
}

void __osMallocInit(Arena* arena, void* start, size_t size) {
    memset(arena,0, sizeof(Arena));
    ArenaImpl_LockInit(arena);
    __osMallocAddBlock(arena, start, size);
    arena->isInit = true;
}

void __osMallocAddBlock(Arena* arena, void* start, ptrdiff_t size) {
    s32 diff;
    ptrdiff_t size2;
    ArenaNode* firstNode;
    ArenaNode* lastNode;

    if (start != NULL) {
        firstNode = (ArenaNode*)ALIGN16((uintptr_t)start);
        diff = (uintptr_t)firstNode - (uintptr_t)start;
        size2 = (size - diff) & ~0xF;

        if (size2 > (ptrdiff_t)sizeof(ArenaNode)) {
            memset(firstNode, BLOCK_UNINIT_MAGIC, size2); // memset
            firstNode->next = NULL;
            firstNode->prev = NULL;
            firstNode->size = size2 - sizeof(ArenaNode);
            firstNode->isFree = true;
            firstNode->magic = NODE_MAGIC;
            ArenaImpl_Lock(arena);
            lastNode = ArenaImpl_GetLastBlock(arena);
            if (lastNode == NULL) {
                arena->head = firstNode;
                arena->start = start;
            } else {
                firstNode->prev = lastNode;
                lastNode->next = firstNode;
            }
            ArenaImpl_Unlock(arena);
        }
    }
}

void ArenaImpl_RemoveAllBlocks(Arena* arena) {
    ArenaNode* iter;
    ArenaNode* next;

    ArenaImpl_Lock(arena);

    iter = arena->head;
    while (iter != NULL) {
        next = ArenaImpl_GetNextBlock(iter);
        memset(iter, BLOCK_UNINIT_MAGIC, iter->size + sizeof(ArenaNode)); // memset
        iter = next;
    }

    ArenaImpl_Unlock(arena);
}

void __osMallocCleanup(Arena* arena) {
    ArenaImpl_RemoveAllBlocks(arena);
    memset(arena, 0, sizeof(*arena));
}

u8 __osMallocIsInitalized(Arena* arena) {
    return arena->isInit;
}

void __osMalloc_FreeBlockTest(Arena* arena, ArenaNode* node) {
    ArenaNode* node2 = node;
    u32* start;
    u32* end;
    u32* iter;

    if (__osMalloc_FreeBlockTest_Enable) {
        start = (u32*)((uintptr_t)node + sizeof(ArenaNode));
        end = (u32*)((uintptr_t)start + node2->size);
        iter = start;

        while (iter < end) {
            if (*iter != BLOCK_UNINIT_MAGIC_32 && *iter != BLOCK_FREE_MAGIC_32) {
                osSyncPrintf(
                    VT_COL(RED, WHITE) "緊急事態！メモリリーク検出！ (block=%08x s=%08x e=%08x p=%08x)\n" VT_RST, node,
                    start, end, iter);
                __osDisplayArena(arena);
                return;
            }
            iter++;
        }
    }
}

void* __osMalloc_NoLockDebug(Arena* arena, size_t size, const char* file, s32 line) {
    ArenaNode* iter;
    u32 blockSize;
    ArenaNode* newNode;
    void* alloc = NULL;
    ArenaNode* next;

    iter = arena->head;
    size = ALIGN16(size);
    blockSize = ALIGN16(size) + sizeof(ArenaNode);

    while (iter != NULL) {
        if (iter->isFree && iter->size >= size) {
            if (arena->flag & CHECK_FREE_BLOCK) {
                __osMalloc_FreeBlockTest(arena, iter);
            }

            if (blockSize < iter->size) {
                newNode = (ArenaNode*)((uintptr_t)iter + blockSize);
                newNode->next = ArenaImpl_GetNextBlock(iter);
                newNode->prev = iter;
                newNode->size = iter->size - blockSize;
                newNode->isFree = true;
                newNode->magic = NODE_MAGIC;

                iter->next = newNode;
                iter->size = size;
                next = ArenaImpl_GetNextBlock(newNode);
                if (next) {
                    next->prev = newNode;
                }
            }

            iter->isFree = false;
            //ArenaImpl_SetDebugInfo(iter, file, line, arena);
            alloc = (void*)((uintptr_t)iter + sizeof(ArenaNode));
            if (arena->flag & FILL_ALLOCBLOCK) {
                memset(alloc, BLOCK_ALLOC_MAGIC, size);
            }

            break;
        }

        iter = ArenaImpl_GetNextBlock(iter);
    }

    return alloc;
}

void* __osMallocDebug(Arena* arena, size_t size, const char* file, s32 line) {
    void* alloc;

    ArenaImpl_Lock(arena);
    alloc = __osMalloc_NoLockDebug(arena, size, file, line);
    ArenaImpl_Unlock(arena);

    return alloc;
}

void* __osMallocRDebug(Arena* arena, size_t size, const char* file, s32 line) {
    ArenaNode* iter;
    ArenaNode* newNode;
    u32 blockSize;
    ArenaNode* next;
    void* allocR = NULL;

    size = ALIGN16(size);
    ArenaImpl_Lock(arena);
    iter = ArenaImpl_GetLastBlock(arena);

    while (iter != NULL) {
        if (iter->isFree && iter->size >= size) {
            if (arena->flag & CHECK_FREE_BLOCK) {
                __osMalloc_FreeBlockTest(arena, iter);
            }

            blockSize = ALIGN16(size) + sizeof(ArenaNode);
            if (blockSize < iter->size) {
                newNode = (ArenaNode*)((uintptr_t)iter + (iter->size - size));
                newNode->next = ArenaImpl_GetNextBlock(iter);
                newNode->prev = iter;
                newNode->size = size;
                newNode->magic = NODE_MAGIC;

                iter->next = newNode;
                iter->size -= blockSize;
                next = ArenaImpl_GetNextBlock(newNode);
                if (next) {
                    next->prev = newNode;
                }
                iter = newNode;
            }

            iter->isFree = false;
            //ArenaImpl_SetDebugInfo(iter, file, line, arena);
            allocR = (void*)((uintptr_t)iter + sizeof(ArenaNode));
            if (arena->flag & FILL_ALLOCBLOCK) {
                memset(allocR, BLOCK_ALLOC_MAGIC, size);
            }

            break;
        }

        iter = ArenaImpl_GetPrevBlock(iter);
    }
    ArenaImpl_Unlock(arena);

    return allocR;
}

void* __osMalloc_NoLock(Arena* arena, size_t size) {
    ArenaNode* iter;
    u32 blockSize;
    ArenaNode* newNode;
    void* alloc = NULL;
    ArenaNode* next;

    iter = arena->head;
    size = ALIGN16(size);
    blockSize = ALIGN16(size) + sizeof(ArenaNode);

    while (iter != NULL) {

        if (iter->isFree && iter->size >= size) {
            if (arena->flag & CHECK_FREE_BLOCK) {
                __osMalloc_FreeBlockTest(arena, iter);
            }

            if (blockSize < iter->size) {
                newNode = (ArenaNode*)((uintptr_t)iter + blockSize);
                newNode->next = ArenaImpl_GetNextBlock(iter);
                newNode->prev = iter;
                newNode->size = iter->size - blockSize;
                newNode->isFree = true;
                newNode->magic = NODE_MAGIC;

                iter->next = newNode;
                iter->size = size;
                next = ArenaImpl_GetNextBlock(newNode);
                if (next) {
                    next->prev = newNode;
                }
            }

            iter->isFree = false;
            //ArenaImpl_SetDebugInfo(iter, NULL, 0, arena);
            alloc = (void*)((uintptr_t)iter + sizeof(ArenaNode));
            if (arena->flag & FILL_ALLOCBLOCK) {
                memset(alloc, BLOCK_ALLOC_MAGIC, size);
            }
            break;
        }

        iter = ArenaImpl_GetNextBlock(iter);
    }

    return alloc;
}

void* __osMalloc(Arena* arena, size_t size) {
    void* alloc;

    ArenaImpl_Lock(arena);
    alloc = __osMalloc_NoLock(arena, size);
    ArenaImpl_Unlock(arena);

    return alloc;
}

void* __osMallocR(Arena* arena, size_t size) {
    ArenaNode* iter;
    ArenaNode* newNode;
    u32 blockSize;
    ArenaNode* next;
    void* alloc = NULL;

    size = ALIGN16(size);
    ArenaImpl_Lock(arena);
    iter = ArenaImpl_GetLastBlock(arena);

    while (iter != NULL) {
        if (iter->isFree && iter->size >= size) {
            if (arena->flag & CHECK_FREE_BLOCK) {
                __osMalloc_FreeBlockTest(arena, iter);
            }

            blockSize = ALIGN16(size) + sizeof(ArenaNode);
            if (blockSize < iter->size) {
                newNode = (ArenaNode*)((uintptr_t)iter + (iter->size - size));
                newNode->next = ArenaImpl_GetNextBlock(iter);
                newNode->prev = iter;
                newNode->size = size;
                newNode->magic = NODE_MAGIC;

                iter->next = newNode;
                iter->size -= blockSize;
                next = ArenaImpl_GetNextBlock(newNode);
                if (next) {
                    next->prev = newNode;
                }
                iter = newNode;
            }

            iter->isFree = false;
            //ArenaImpl_SetDebugInfo(iter, NULL, 0, arena);
            alloc = (void*)((uintptr_t)iter + sizeof(ArenaNode));
            if (arena->flag & FILL_ALLOCBLOCK) {
                memset(alloc, BLOCK_ALLOC_MAGIC, size);
            }
            break;
        }
        iter = ArenaImpl_GetPrevBlock(iter);
    }
    ArenaImpl_Unlock(arena);

    return alloc;
}

void __osFree_NoLock(Arena* arena, void* ptr) {
    ArenaNode* node;
    ArenaNode* next;
    ArenaNode* prev;
    ArenaNode* newNext;

    if (ptr == NULL) {
        return;
    }

    node = (ArenaNode*)((uintptr_t)ptr - sizeof(ArenaNode));
    if (node == NULL || node->magic != NODE_MAGIC) {
        // "__osFree: Unauthorized release (%08x)"
        osSyncPrintf(VT_COL(RED, WHITE) "__osFree:不正解放(%08x)\n" VT_RST, ptr);
        return;
    }
    if (node->isFree) {
        osSyncPrintf(VT_COL(RED, WHITE) "__osFree:二重解放(%08x)\n" VT_RST, ptr); // "__osFree: Double release (%08x)"
        return;
    }
    #if 0
    if (arena != node->arena && arena != NULL) {
        // "__osFree:Tried to release in a different way than when it was secured (%08x:%08x)"
        osSyncPrintf(VT_COL(RED, WHITE) "__osFree:確保時と違う方法で解放しようとした (%08x:%08x)\n" VT_RST, arena,
                     node->arena);
        return;
    }
#endif
    next = ArenaImpl_GetNextBlock(node);
    prev = ArenaImpl_GetPrevBlock(node);
    node->isFree = true;
    //ArenaImpl_SetDebugInfo(node, NULL, 0, arena);

    if (arena->flag & FILL_FREEBLOCK) {
        memset((uintptr_t)node + sizeof(ArenaNode), BLOCK_FREE_MAGIC, node->size);
    }

    newNext = next;
    if ((uintptr_t)next == (uintptr_t)node + sizeof(ArenaNode) + node->size && next->isFree) {
        newNext = ArenaImpl_GetNextBlock(next);
        if (newNext != NULL) {
            newNext->prev = node;
        }

        node->size += next->size + sizeof(ArenaNode);
        if (arena->flag & FILL_FREEBLOCK) {
            memset(next, BLOCK_FREE_MAGIC, sizeof(ArenaNode));
        }
        node->next = newNext;
        next = newNext;
    }

    if (prev != NULL && prev->isFree && (uintptr_t)node == (uintptr_t)prev + sizeof(ArenaNode) + prev->size) {
        if (next) {
            next->prev = prev;
        }
        prev->next = next;
        prev->size += node->size + sizeof(ArenaNode);
        if (arena->flag & FILL_FREEBLOCK) {
            memset(node, BLOCK_FREE_MAGIC, sizeof(ArenaNode));
        }
    }
}

void __osFree(Arena* arena, void* ptr) {
    ArenaImpl_Lock(arena);
    __osFree_NoLock(arena, ptr);
    ArenaImpl_Unlock(arena);
}

void __osFree_NoLockDebug(Arena* arena, void* ptr, const char* file, s32 line) {
    ArenaNode* node;
    ArenaNode* next;
    ArenaNode* prev;
    ArenaNode* newNext;

    if (ptr == NULL) {
        return;
    }

    node = (ArenaNode*)((uintptr_t)ptr - sizeof(ArenaNode));
    if (node == NULL || node->magic != NODE_MAGIC) {
        // "__osFree: Unauthorized release (%08x)"
        osSyncPrintf(VT_COL(RED, WHITE) "__osFree:不正解放(%08x) [%s:%d ]\n" VT_RST, ptr, file, line);
        return;
    }
    if (node->isFree) {
        // "__osFree: Double release (%08x)"
        osSyncPrintf(VT_COL(RED, WHITE) "__osFree:二重解放(%08x) [%s:%d ]\n" VT_RST, ptr, file, line);
        return;
    }
    #if 0
    if (arena != node->arena && arena != NULL) {
        // "__osFree:Tried to release in a different way than when it was secured (%08x:%08x)"
        osSyncPrintf(VT_COL(RED, WHITE) "__osFree:確保時と違う方法で解放しようとした (%08x:%08x)\n" VT_RST, arena,
                     node->arena);
        return;
    }
    #endif
    next = ArenaImpl_GetNextBlock(node);
    prev = ArenaImpl_GetPrevBlock(node);
    node->isFree = true;
    //ArenaImpl_SetDebugInfo(node, file, line, arena);

    if (arena->flag & FILL_FREEBLOCK) {
        memset((uintptr_t)node + sizeof(ArenaNode), BLOCK_FREE_MAGIC, node->size);
    }

    newNext = node->next;
    if ((uintptr_t)next == (uintptr_t)node + sizeof(ArenaNode) + node->size && next->isFree) {
        newNext = ArenaImpl_GetNextBlock(next);
        if (newNext != NULL) {
            newNext->prev = node;
        }

        node->size += next->size + sizeof(ArenaNode);
        if (arena->flag & FILL_FREEBLOCK) {
            memset(next, BLOCK_FREE_MAGIC, sizeof(ArenaNode));
        }
        node->next = newNext;
        next = newNext;
    }

    if (prev != NULL && prev->isFree && (uintptr_t)node == (uintptr_t)prev + sizeof(ArenaNode) + prev->size) {
        if (next != NULL) {
            next->prev = prev;
        }
        prev->next = next;
        prev->size += node->size + sizeof(ArenaNode);
        if (arena->flag & FILL_FREEBLOCK) {
            memset(node, BLOCK_FREE_MAGIC, sizeof(ArenaNode));
        }
    }
}

void __osFreeDebug(Arena* arena, void* ptr, const char* file, s32 line) {
    ArenaImpl_Lock(arena);
    __osFree_NoLockDebug(arena, ptr, file, line);
    ArenaImpl_Unlock(arena);
}

void* __osRealloc(Arena* arena, void* ptr, size_t newSize) {
    void* newAlloc;
    ArenaNode* node;
    ArenaNode* next;
    ArenaNode* newNext;
    ArenaNode* overNext;
    ArenaNode* newNext2;
    ArenaNode* next2;
    size_t sizeDiff;
    ArenaNode* overNext2;
    ArenaNode localCopy;
    u32 blockSize;
    s32 pad;

    newSize = ALIGN16(newSize);
    osSyncPrintf("__osRealloc(%08x, %d)\n", ptr, newSize);
    ArenaImpl_Lock(arena);

    if (ptr == NULL) {
        ptr = __osMalloc_NoLock(arena, newSize);
    } else if (newSize == 0) {
        __osFree_NoLock(arena, ptr);
        ptr = NULL;
    } else {
        node = (ArenaNode*)((uintptr_t)ptr - sizeof(ArenaNode));
        if (newSize == node->size) {
            // "Does nothing because the memory block size does not change"
            osSyncPrintf("メモリブロックサイズが変わらないためなにもしません\n");
        } else if (node->size < newSize) {
            next = ArenaImpl_GetNextBlock(node);
            sizeDiff = newSize - node->size;
            if ((uintptr_t)next == ((uintptr_t)node + node->size + sizeof(ArenaNode)) && next->isFree && next->size >= sizeDiff) {
                // "Merge because there is a free block after the current memory block"
                osSyncPrintf("現メモリブロックの後ろにフリーブロックがあるので結合します\n");
                next->size -= sizeDiff;
                overNext = ArenaImpl_GetNextBlock(next);
                newNext = (ArenaNode*)((uintptr_t)next + sizeDiff);
                if (overNext != NULL) {
                    overNext->prev = newNext;
                }
                node->next = newNext;
                node->size = newSize;
                func_801068B0(newNext, next, sizeof(ArenaNode)); // memcpy
            } else {
                // "Allocate a new memory block and move the contents"
                osSyncPrintf("新たにメモリブロックを確保して内容を移動します\n");
                newAlloc = __osMalloc_NoLock(arena, newSize);
                if (newAlloc != NULL) {
                    memcpy(newAlloc, ptr, node->size);
                    __osFree_NoLock(arena, ptr);
                }
                ptr = newAlloc;
            }
        } else if (newSize < node->size) {
            next2 = ArenaImpl_GetNextBlock(node);
            if (next2 != NULL && next2->isFree) {
                blockSize = ALIGN16(newSize) + sizeof(ArenaNode);
                // "Increased free block behind current memory block"
                osSyncPrintf("現メモリブロックの後ろのフリーブロックを大きくしました\n");
                newNext2 = (ArenaNode*)((uintptr_t)node + blockSize);
                localCopy = *next2;
                *newNext2 = localCopy;
                newNext2->size += node->size - newSize;
                node->next = newNext2;
                node->size = newSize;
                overNext2 = ArenaImpl_GetNextBlock(newNext2);
                if (overNext2 != NULL) {
                    overNext2->prev = newNext2;
                }
            } else if (newSize + sizeof(ArenaNode) < node->size) {
                blockSize = ALIGN16(newSize) + sizeof(ArenaNode);
                // "Generated because there is no free block after the current memory block"
                osSyncPrintf("現メモリブロックの後ろにフリーブロックがないので生成します\n");
                newNext2 = (ArenaNode*)((uintptr_t)node + blockSize);
                newNext2->next = ArenaImpl_GetNextBlock(node);
                newNext2->prev = node;
                newNext2->size = node->size - blockSize;
                newNext2->isFree = true;
                newNext2->magic = NODE_MAGIC;
                node->next = newNext2;
                node->size = newSize;
                overNext2 = ArenaImpl_GetNextBlock(newNext2);
                if (overNext2 != NULL) {
                    overNext2->prev = newNext2;
                }
            } else {
                // "There is no room to generate free blocks"
                osSyncPrintf("フリーブロック生成するだけの空きがありません\n");
                ptr = NULL;
            }
        }
    }
    ArenaImpl_Unlock(arena);

    return ptr;
}

void* __osReallocDebug(Arena* arena, void* ptr, size_t newSize, const char* file, s32 line) {
    return __osRealloc(arena, ptr, newSize);
}

void ArenaImpl_GetSizes(Arena* arena, u32* outMaxFree, u32* outFree, u32* outAlloc) {
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

        iter = ArenaImpl_GetNextBlock(iter);
    }

    ArenaImpl_Unlock(arena);
}

void __osDisplayArena(Arena* arena) {
    size_t freeSize;
    size_t allocatedSize;
    size_t maxFree;
    ArenaNode* iter;
    ArenaNode* next;

    if (!__osMallocIsInitalized(arena)) {
        osSyncPrintf("アリーナは初期化されていません\n"); // "Arena is not initalized"
        return;
    }

    ArenaImpl_Lock(arena);

    maxFree = 0;
    freeSize = 0;
    allocatedSize = 0;

    osSyncPrintf("アリーナの内容 (0x%08x)\n", arena); // "Arena contents (0x%08x)"
    // "Memory node range status size [time s ms us ns: TID: src: line]"
    osSyncPrintf("メモリブロック範囲 status サイズ  [時刻  s ms us ns: TID:src:行]\n");

    iter = arena->head;
    while (iter != NULL) {
        if (iter != NULL && iter->magic == NODE_MAGIC) {
            next = iter->next;
            osSyncPrintf("%08x-%08x%c %s %08x", iter, ((uintptr_t)iter + sizeof(ArenaNode) + iter->size),
                         (next == NULL) ? '$' : (iter != next->prev ? '!' : ' '),
                         iter->isFree ? "空き" : "確保", //? "Free" : "Secure"
                         iter->size);
            #if 0
            if (!iter->isFree) {
                osSyncPrintf(" [%016llu:%2d:%s:%d]", OS_CYCLES_TO_NSEC(iter->time), iter->threadId,
                             iter->filename != NULL ? iter->filename : "**NULL**", iter->line);
            }
            #endif
            osSyncPrintf("\n");

            if (iter->isFree) {
                freeSize += iter->size;
                if (maxFree < iter->size) {
                    maxFree = iter->size;
                }
            } else {
                allocatedSize += iter->size;
            }
        } else {
            osSyncPrintf("%08x Block Invalid\n", iter);
            next = NULL;
        }
        iter = next;
    }

    // "Total reserved node size 0x%08x bytes"
    osSyncPrintf("確保ブロックサイズの合計 0x%08x バイト\n", allocatedSize);
    // "Total free node size 0x%08x bytes"
    osSyncPrintf("空きブロックサイズの合計 0x%08x バイト\n", freeSize);
    // "Maximum free node size 0x%08x bytes"
    osSyncPrintf("最大空きブロックサイズ   0x%08x バイト\n", maxFree);

    ArenaImpl_Unlock(arena);
}

void ArenaImpl_FaultClient(Arena* arena) {
    size_t freeSize;
    size_t allocatedSize;
    size_t maxFree;
    ArenaNode* iter;
    ArenaNode* next;

    FaultDrawer_Printf("ARENA INFO (0x%08x)\n", arena);
    if (!__osMallocIsInitalized(arena)) {
        FaultDrawer_Printf("Arena is uninitalized\n", arena);
        return;
    }

    maxFree = 0;
    freeSize = 0;
    allocatedSize = 0;

    FaultDrawer_Printf("Memory Block Region status size\n");

    iter = arena->head;
    while (iter != NULL) {
        if (iter != NULL && iter->magic == NODE_MAGIC) {
            next = iter->next;
            FaultDrawer_Printf("%08x-%08x%c %s %08x", iter, ((uintptr_t)iter + sizeof(ArenaNode) + iter->size),
                               (!next) ? '$' : (iter != next->prev ? '!' : ' '), iter->isFree ? "F" : "A", iter->size);

            FaultDrawer_Printf("\n");

            if (iter->isFree) {
                freeSize += iter->size;
                if (maxFree < iter->size) {
                    maxFree = iter->size;
                }
            } else {
                allocatedSize += iter->size;
            }
        } else {
            FaultDrawer_SetFontColor(0xF801);
            FaultDrawer_Printf("%08x Block Invalid\n", iter);
            next = NULL;
        }
        iter = next;
    }

    FaultDrawer_SetFontColor(0x7F1);
    FaultDrawer_Printf("Total Alloc Block Size  %08x\n", allocatedSize);
    FaultDrawer_Printf("Total Free Block Size   %08x\n", freeSize);
    FaultDrawer_Printf("Largest Free Block Size %08x\n", maxFree);
}

u32 __osCheckArena(Arena* arena) {
    ArenaNode* iter;
    u32 error = 0;

    ArenaImpl_Lock(arena);
    // "Checking the contents of the arena. . ． (%08x)"
    osSyncPrintf("アリーナの内容をチェックしています．．． (%08x)\n", arena);
    iter = arena->head;
    while (iter != NULL) {
        if (iter && iter->magic == NODE_MAGIC) {
            // "Oops!! (%08x %08x)"
            osSyncPrintf(VT_COL(RED, WHITE) "おおっと！！ (%08x %08x)\n" VT_RST, iter, iter->magic);
            error = 1;
            break;
        }
        iter = ArenaImpl_GetNextBlock(iter);
    }
    if (error == 0) {
        osSyncPrintf("アリーナはまだ、いけそうです\n"); // "The arena is still going well"
    }
    ArenaImpl_Unlock(arena);

    return error;
}

u8 func_800FF334(Arena* arena) {
    return arena->unk_20;
}

#ifndef OS_MALLOC_H
#define OS_MALLOC_H

#include "PR/ultratypes.h"
#include "PR/os_message.h"
#include "libc/stddef.h"

typedef struct ArenaNode {
    /* 0x0 */ s16 magic; // Should always be 0x7373
    /* 0x2 */ s16 isFree;
    /* 0x4 */ size_t size;
    /* 0x8 */ struct ArenaNode* next;
    /* 0xC */ struct ArenaNode* prev;
} ArenaNode; // size = 0x10

typedef struct {
    /* 0x00 */ ArenaNode* head;
    /* 0x04 */ void* start;
    /* 0x08 */ OSMesgQueue lock;
    /* 0x20 */ u8 unk20;
    /* 0x21 */ u8 isInit;
    /* 0x22 */ u8 flag;
} Arena; // size = 0x24

void __osMallocInit(Arena* arena, void* heap, size_t size);
void __osMallocCleanup(Arena* arena);
u8 __osMallocIsInitalized(Arena* arena);
void* __osMalloc(Arena* arena, size_t size);
void* __osMallocR(Arena* arena, size_t size);
void __osFree(Arena* arena, void* ptr);
void* __osRealloc(Arena* arena, void* ptr, size_t newSize);
void __osGetSizes(Arena* arena, size_t* outMaxFree, size_t* outFree, size_t* outAlloc);
s32 __osCheckArena(Arena* arena);

#endif

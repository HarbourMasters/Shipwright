#ifndef THA_H
#define THA_H

#include "ultra64.h"
#include "libc/stdint.h"
#include "libc/stddef.h"

typedef struct TwoHeadArena {
    /* 0x0 */ size_t size;
    /* 0x4 */ void* start;
    /* 0x8 */ void* head;
    /* 0xC */ void* tail;
} TwoHeadArena; // size = 0x10

void* THA_GetHead(TwoHeadArena* tha);
void THA_SetHead(TwoHeadArena* tha, void* newHead);
void* THA_GetTail(TwoHeadArena* tha);
void* THA_AllocHead(TwoHeadArena* tha, size_t size);
void* THA_AllocHeadByte(TwoHeadArena* tha);
void* THA_AllocTail(TwoHeadArena* tha, size_t size);
void* THA_AllocTailAlign16(TwoHeadArena* tha, size_t size);
void* THA_AllocTailAlign(TwoHeadArena* tha, size_t size, uintptr_t mask);
ptrdiff_t THA_GetRemaining(TwoHeadArena* tha);
u32 THA_IsCrash(TwoHeadArena* tha);
void THA_Reset(TwoHeadArena* tha);
void THA_Init(TwoHeadArena* tha, void* start, size_t size);
void THA_Destroy(TwoHeadArena* tha);

#endif

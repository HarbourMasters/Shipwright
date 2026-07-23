#ifndef THA_H
#define THA_H

#include <libultraship/libultra.h>

#include "extern_c_helper.h"

// TODO: Update the type to upstream
typedef struct TwoHeadArena {
    /* 0x00 */ u32 size;
    /* 0x04 */ void* bufp;
    /* 0x08 */ void* head;
    /* 0x0C */ void* tail;
} TwoHeadArena; // size = 0x10

// TODO: Update the names to upstream
EXTERN_C void* THA_GetHead(TwoHeadArena* tha);
EXTERN_C void THA_SetHead(TwoHeadArena* tha, void* start);
EXTERN_C void* THA_GetTail(TwoHeadArena* tha);
EXTERN_C void* THA_AllocStart(TwoHeadArena* tha, size_t size);
EXTERN_C void* THA_AllocStart1(TwoHeadArena* tha);
EXTERN_C void* THA_AllocEnd(TwoHeadArena* tha, size_t size);
EXTERN_C void* THA_AllocEndAlign16(TwoHeadArena* tha, size_t size);
EXTERN_C void* THA_AllocEndAlign(TwoHeadArena* tha, size_t size, size_t mask);
EXTERN_C s32 THA_GetSize(TwoHeadArena* tha);
EXTERN_C u32 THA_IsCrash(TwoHeadArena* tha);
EXTERN_C void THA_Init(TwoHeadArena* tha);
EXTERN_C void THA_Ct(TwoHeadArena* tha, void* ptr, size_t size);
EXTERN_C void THA_Dt(TwoHeadArena* tha);

#endif
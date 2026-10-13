#ifndef LISTALLOC_H
#define LISTALLOC_H

#ifdef __cplusplus
extern "C" {
#define this thisx
#endif

#include "ultra64.h"

typedef struct ListAlloc {
    /* 0x0 */ struct ListAlloc* prev;
    /* 0x4 */ struct ListAlloc* next;
} ListAlloc; // size = 0x8

ListAlloc* ListAlloc_Init(ListAlloc* this);
void* ListAlloc_Alloc(ListAlloc* this, size_t size);
void ListAlloc_Free(ListAlloc* this, void* data);
void ListAlloc_FreeAll(ListAlloc* this);

#ifdef __cplusplus
}
#undef this
#endif

#endif
#ifndef SYSTEM_HEAP_H
#define SYSTEM_HEAP_H

#include "libc/stddef.h"
#include "PR/ultratypes.h"

void* SystemHeap_Malloc(size_t size);
void SystemHeap_Free(void* ptr);
void SystemHeap_Init(void* start, size_t size);

#endif

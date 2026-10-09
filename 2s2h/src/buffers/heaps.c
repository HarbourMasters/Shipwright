#include "buffers.h"
#include "z64.h"
#include <assert.h>
#include <stdlib.h>
#if defined(__unix__) || defined(__APPLE__)
#include <sys/mman.h>
#endif

u8* gAudioHeap;
u8* gSystemHeap;

void Heaps_Alloc(void) {
#ifdef _MSC_VER
    gAudioHeap = (u8*)_aligned_malloc(AUDIO_HEAP_SIZE, 0x10);
    gSystemHeap = (u8*)_aligned_malloc(SYSTEM_HEAP_SIZE, 0x10);
#elif defined(__unix__) || defined(__APPLE__)
    gAudioHeap = mmap(NULL, AUDIO_HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    gSystemHeap = mmap(NULL, SYSTEM_HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    assert(gAudioHeap != MAP_FAILED);
    assert(gSystemHeap != MAP_FAILED);
#else
    gAudioHeap = (u8*)memalign(0x10, AUDIO_HEAP_SIZE);
    gSystemHeap = (u8*)memalign(0x10, SYSTEM_HEAP_SIZE);
#endif

    assert(gAudioHeap != NULL);
    assert(gSystemHeap != NULL);
}

void Heaps_Free(void) {
#ifdef _MSC_VER
    _aligned_free(gAudioHeap);
    _aligned_free(gSystemHeap);
#elif defined(__unix__) || defined(__APPLE__)
    munmap(gAudioHeap, AUDIO_HEAP_SIZE);
    munmap(gSystemHeap, SYSTEM_HEAP_SIZE);
#else
    free(gAudioHeap);
    free(gSystemHeap);
#endif
}

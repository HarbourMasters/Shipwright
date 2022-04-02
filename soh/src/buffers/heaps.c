#include "z64.h"
#include <assert.h>
#include <malloc.h>

#ifndef _MSC_VER
#include <unistd.h>
#endif

#define AUDIO_HEAP_SIZE 0x38000
#define SYSTEM_HEAP_SIZE (1024 * 1024 * 128)

u8* gAudioHeap;

u8* gSystemHeap;

void Heaps_Alloc(void)
{
#ifdef _MSC_VER
    gAudioHeap = (u8*)_aligned_malloc(AUDIO_HEAP_SIZE, 16);
    gSystemHeap = (u8*)_aligned_malloc(SYSTEM_HEAP_SIZE, 16);
#elif defined(_POSIX_VERSION) && (_POSIX_VERSION >= 200112L)
    if (posix_memalign((void**)&gAudioHeap, 16, AUDIO_HEAP_SIZE) != 0)
        gAudioHeap = NULL;
    if (posix_memalign((void**)&gSystemHeap, 16, SYSTEM_HEAP_SIZE) != 0)
        gSystemHeap = NULL;
#else
    gAudioHeap = (u8*)memalign(16, AUDIO_HEAP_SIZE);
    gSystemHeap = (u8*)memalign(16, SYSTEM_HEAP_SIZE);
#endif

    assert(gAudioHeap != NULL);
    assert(gSystemHeap != NULL);
}

void Heaps_Free(void)
{
#ifdef _MSC_VER
    _aligned_free(gAudioHeap);
    _aligned_free(gSystemHeap);
#else
    free(gAudioHeap);
    free(gSystemHeap);
#endif
}

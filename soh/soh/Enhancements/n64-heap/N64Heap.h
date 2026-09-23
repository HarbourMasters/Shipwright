#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void N64Heap_OnArenaInit(void);
void N64Heap_OnAlloc(void* ptr, size_t size, const char* file, int reverse);
void N64Heap_OnFree(void* ptr);
// Returns 0 when the spawn would not fit in the N64 heap and "Fix N64 Heap" is enabled
int N64Heap_ActorSpawn(int16_t actorId);
int N64Heap_EffectSpawn(int32_t type);
// Returns a script that behaves like the N64 data at a stale cutscene pointer, or `script`
void* N64Heap_FilterCutsceneScript(void* script);
int N64Heap_HasN64Data(void);

#ifdef __cplusplus
}
#endif

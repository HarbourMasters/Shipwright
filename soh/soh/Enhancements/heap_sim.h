#ifndef HEAP_SIM_H
#define HEAP_SIM_H

#include "global.h"
#include "soh/ActorDB.h"

#ifdef __cplusplus
extern "C" {
#endif

// Heap sim: heap pressure simulator. Mirrors every ZeldaArena/GameState
// allocation into a shadow arena sized like N64 heap.

void HeapSim_BeginSimulation(void);
void HeapSim_InitArena(GameState* gameState);
void HeapSim_Cleanup(void);
s32 HeapSim_MirrorMalloc(void* ptr, size_t size);
s32 HeapSim_MirrorMallocR(void* ptr, size_t size);
void HeapSim_MirrorFree(void* ptr);
void HeapSim_MirrorGameAlloc(void* ptr, size_t size);
void HeapSim_MirrorMapMarkLoad(void);
s32 HeapSim_MirrorEffectSsOverlay(s32 type);
s32 HeapSim_RegisterActorOverlayIfNeeded(ActorDBEntry* dbEntry);
void HeapSim_UnregisterActorOverlayIfNeeded(ActorDBEntry* dbEntry);
void HeapSim_FreeAbsoluteSpace(void);

#ifdef __cplusplus
}
#endif

#endif // HEAP_SIM_H

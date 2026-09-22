#pragma once

// C interface for the N64 actor-heap shadow (see N64HeapCore.h).
// Called from src/code/z_malloc.c; everything else lives in this folder.

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void N64Heap_OnArenaInit(void);
void N64Heap_OnAlloc(void* ptr, size_t size, const char* file, int reverse);
void N64Heap_OnFree(void* ptr);

// Called by Cutscene_ProcessCommands with the script it is about to run.
// With the CVar gEnhancements.FixWrongWarps enabled (Enhancements > Fixes >
// Gameplay Fixes > "Fix Wrong Warps"), and when NTSC 1.2
// memory at the cutscene pointer no longer holds this script (a stale
// pointer, as in a wrong warp), returns a script that behaves like the N64
// bytes there. Otherwise returns `script` unchanged.
void* N64Heap_FilterCutsceneScript(void* script);

// True when oot.o2r contains the N64 data the model reads. The extractor only
// adds it for NTSC 1.2 ROMs (soh/assets/yml/ntsc_1-2/n64heap), the version
// everything here is modelled on.
int N64Heap_HasN64Data(void);

#ifdef __cplusplus
}
#endif

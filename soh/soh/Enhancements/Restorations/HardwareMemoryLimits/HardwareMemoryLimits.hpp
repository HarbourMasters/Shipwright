#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {

#endif

// N64 subsidiary struct sizes (32-bit, from decomp headers and linker map).
#define N64_SIZEOF_COLLIDER_JNT_SPH_ELEM 0x40
#define N64_SIZEOF_COLLIDER_TRIS_ELEM    0x5C
#define N64_SIZEOF_CAMERA                0x16C
#define N64_SIZEOF_SKIN_LIMB_VTX         0x0C

// --------------------------------------------------------------------------------------------------------------------
// Lifecycle
// --------------------------------------------------------------------------------------------------------------------

// Store SoH's THA remainder before ZeldaArena_Init consumes it.  Call from Play_Init immediately after
// THA_GetRemaining.
void N64Mem_StoreThaRemainder(uint32_t sohRemainder);

// Store which elf message file was loaded by Scene_CommandSpecialFiles (1 = elf_message_field, 2 = elf_message_ydan,
// 0 = none).  Called from z_scene.c during scene command processing.
void N64Mem_StoreElfMsgNum(uint8_t num);

// Returns the elf message number stored by N64Mem_StoreElfMsgNum.
uint8_t N64Mem_GetElfMsgNum(void);

// Reset shadow state, compute N64-equivalent arena size from stored THA remainder minus N64-specific consumers
// (room buffers, etc.), and reread CVar.  Call from Play_Init after ZeldaArena_Init.
struct PlayState;
void N64Mem_Reset(PlayState* play);

// Returns whether the N64 memory model is currently active.
int32_t N64Mem_IsActive(void);

// Returns the shadow arena pointer for debug visualization.  Only valid when N64Mem_IsActive() is true.
struct ShadowArena* N64Mem_GetShadowArena(void);

// Log the current shadow arena state (alloc/free/largest) with the given context label.
void N64Mem_LogState(const char* context);

// --------------------------------------------------------------------------------------------------------------------
// Enemy randomizer compatibility
//
// When the enemy randomizer replaces an actor, the shadow arena should charge the ORIGINAL actor's N64 sizes (overlay
// and instance) rather than the replacement's.  This preserves authentic N64 heap geometry -- memory-dependent
// behaviors (SRM, ACE, spawn failure thresholds) remain consistent regardless of which enemies are on screen.
//
// Call SetOriginalActorId with the scene's original actor ID before Actor_Spawn, and ClearOriginalActorId after
// Actor_Spawn returns.  When set, overlay and instance size lookups use the original ID; all other tracking (overlay
// ref counting, instance pointer mapping) uses the actual spawned actor ID.
// --------------------------------------------------------------------------------------------------------------------

void N64Mem_SetOriginalActorId(int16_t actorId);
void N64Mem_ClearOriginalActorId(void);

// Save/restore for the randomized-init skip flag.  Actor_Spawn saves the current value at entry and restores it
// after Actor_Init returns, so child spawns during a randomized actor's init don't clobber the parent's flag.
int32_t N64Mem_GetRandomizedInit(void);
void N64Mem_SetRandomizedInit(int32_t value);

// Log Graveyard benchmark data (transition count, largest_free, total_free).  Call after room actors are spawned.
void N64Mem_BenchmarkTransition(PlayState* play);

// --------------------------------------------------------------------------------------------------------------------
// Actor overlays: Keyed by actor ID, shadow-only allocations.
//
// On N64, overlays load into ZeldaArena from ROM on first spawn and free when no instances remain. SoH compiles them
// into the binary, so they never touch ZeldaArena.  The shadow restores this pressure.
//
// Call AllocOverlay when numLoaded transitions 0 -> true.
// Call FreeOverlay when numLoaded transitions 1 -> false.
// --------------------------------------------------------------------------------------------------------------------

int32_t N64Mem_AllocOverlay(int16_t actorId, uint16_t allocType);
void N64Mem_FreeOverlay(int16_t actorId, uint16_t allocType);

// --------------------------------------------------------------------------------------------------------------------
// Actor instances: Paired with real ZeldaArena allocations.
//
// Call AllocInstance after the real allocation succeeds.  If the shadow cannot satisfy the N64-sized allocation,
// returns false and the caller should treat the spawn as failed.
//
// Call FreeInstance when the actor is deleted.
// --------------------------------------------------------------------------------------------------------------------

int32_t N64Mem_AllocInstance(int16_t actorId, int16_t params, void* realPtr);
// Returns the stored original actor ID for overlay free-path tracking, or -1 if not tracked.
int16_t N64Mem_FreeInstance(void* realPtr);

// --------------------------------------------------------------------------------------------------------------------
// Subsidiaries (colliders, camera, skin, etc.): Paired.
//
// Same pattern as instances -- shadow-alloc at N64 size, gate on failure, free when the real allocation is freed.
// --------------------------------------------------------------------------------------------------------------------

int32_t N64Mem_AllocSubsidiary(void* realPtr, uint32_t n64Size);
void N64Mem_FreeSubsidiary(void* realPtr);

// --------------------------------------------------------------------------------------------------------------------
// Effect overlays: Shadow-only, persist for scene lifetime.
//
// On N64, effect overlays load via MallocR on first spawn and are never freed until the GameState is torn down.
// --------------------------------------------------------------------------------------------------------------------

int32_t N64Mem_AllocEffectOverlay(int32_t type);

// --------------------------------------------------------------------------------------------------------------------
// Heap viewer metadata
//
// Query block identity from the shadow arena.  Returns true if metadata exists for the given data offset, false if
// not.  Block type constants identify the allocation category; actorId is the original (pre-randomizer) actor ID for
// instance/overlay blocks, or -1 for subsidiary/effect/absolute blocks.
// --------------------------------------------------------------------------------------------------------------------

#define N64MEM_BLOCK_FREE       0
#define N64MEM_BLOCK_INSTANCE   1
#define N64MEM_BLOCK_OVERLAY    2
#define N64MEM_BLOCK_SUBSIDIARY 3
#define N64MEM_BLOCK_EFFECT     4
#define N64MEM_BLOCK_ABSOLUTE   5

int32_t N64Mem_GetBlockInfo(uint32_t dataOffset, uint8_t* outType, int16_t* outActorId);

#ifdef __cplusplus
}
#endif
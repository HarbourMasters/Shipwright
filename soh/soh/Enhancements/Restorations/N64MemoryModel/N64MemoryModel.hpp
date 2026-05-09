#pragma once

#include <libultraship/libultraship.h>

#ifdef __cplusplus
extern "C" {
#endif

// N64 subsidiary struct sizes (32-bit, from decomp build artifacts).
#define N64_SIZEOF_COLLIDER_JNT_SPH_ELEM 0x40
#define N64_SIZEOF_COLLIDER_TRIS_ELEM 0x5C
#define N64_SIZEOF_CAMERA 0x16C
#define N64_SIZEOF_SKIN_LIMB_VTX 0x0C
#define N64_SIZEOF_GI_OBJECT_SEGMENT 0x1008

// --------------------------------------------------------------------------------------------------------------------
// Lifecycle
// --------------------------------------------------------------------------------------------------------------------

// Store SoH's THA remainder before ZeldaArena_Init consumes it.  Call from Play_Init immediately after
// THA_GetRemaining.
void N64Mem_StoreThaRemainder(u32 sohRemainder);

// Store which elf message file was loaded by Scene_CommandSpecialFiles (1 = elf_message_field, 2 = elf_message_ydan,
// 0 = none).  Called from z_scene.c during scene command processing.
void N64Mem_StoreElfMsgNum(u8 num);

// Returns the elf message number stored by N64Mem_StoreElfMsgNum.
u8 N64Mem_GetElfMsgNum(void);

// Reset shadow state, compute N64-equivalent arena size from stored THA remainder minus N64-specific consumers
// (room buffers, etc.), and reread CVar.  Call from Play_Init after ZeldaArena_Init.
struct PlayState;
void N64Mem_Reset(PlayState* play);

// Returns whether the N64 memory model is currently active.
s32 N64Mem_IsActive(void);

// Returns the shadow arena pointer for debug visualization.  Only valid when N64Mem_IsActive() is true.
struct ShadowArena* N64Mem_GetShadowArena(void);

// Log the current shadow arena state (alloc/free/largest) with the given context label.
void N64Mem_LogState(const char* context);

// --------------------------------------------------------------------------------------------------------------------
// Actor overlays: Keyed by actor ID, shadow-only allocations.
//
// On N64, overlays load into ZeldaArena from ROM on first spawn and free when no instances remain. SoH compiles them
// into the binary, so they never touch ZeldaArena.  The shadow restores this pressure.
//
// Call AllocOverlay when numLoaded transitions 0 -> 1.
// Call FreeOverlay when numLoaded transitions 1 -> 0.
// --------------------------------------------------------------------------------------------------------------------

s32 N64Mem_AllocOverlay(s16 actorId, u16 allocType);
void N64Mem_FreeOverlay(s16 actorId, u16 allocType);

// --------------------------------------------------------------------------------------------------------------------
// Actor instances: Paired with real ZeldaArena allocations.
//
// Call AllocInstance after the real allocation succeeds.  If the shadow cannot satisfy the N64-sized allocation,
// returns 0 and the caller should treat the spawn as failed.
//
// Call FreeInstance when the actor is deleted.
// --------------------------------------------------------------------------------------------------------------------

s32 N64Mem_AllocInstance(s16 actorId, void* realPtr);
void N64Mem_FreeInstance(void* realPtr);

// --------------------------------------------------------------------------------------------------------------------
// Subsidiaries (colliders, camera, skin, etc.): Paired.
//
// Same pattern as instances -- shadow-alloc at N64 size, gate on failure, free when the real allocation is freed.
// --------------------------------------------------------------------------------------------------------------------

s32 N64Mem_AllocSubsidiary(void* realPtr, u32 n64Size);
void N64Mem_FreeSubsidiary(void* realPtr);

// --------------------------------------------------------------------------------------------------------------------
// Effect overlays: Shadow-only, persist for scene lifetime.
//
// On N64, effect overlays load via MallocR on first spawn and are never freed until the GameState is torn down.
// --------------------------------------------------------------------------------------------------------------------

s32 N64Mem_AllocEffectOverlay(s32 type);

#ifdef __cplusplus
}
#endif

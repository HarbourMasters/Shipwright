#pragma once

#include "z64.h"

#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------------------------------------------------------
// Per version constants that cannot be derived at SoH runtime.
//
// These come from the decomp linker map or DMA table for a given ROM version.  SoH zeroes out or bypasses the code
// paths that would expose them (e.g, kaleido overlays compiled in, skybox textures loaded from OTR, scene files loaded
// via ResourceManager instead of THA, etc.)
//
// #TODO: For multi-version support, generate one of these per supported ROM version.
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    u32 kaleidoOverlayVramSize; // max(ovl_kaleido_scope VRAM, ovl_player_actor VRAM)
    u32 parameterStaticSize; // _parameter_staticSegmentRomEnd - RomStart
    u32 effectSsSize; // N64 sizeof(EffectSs) -- 0x60 on N64, larger on SoH
} VersionConstants;


// Hardcoded version constants for NTSC 1.2.
extern const VersionConstants gVersionConstantsNtsc12;

// --------------------------------------------------------------------------------------------------------------------
// Arena size computation
//
// Computes the N64 ZeldaArena size from first principles:
//  arena = THA_BUDGET - sum(all THA consumers at N64 sizes)
//
// Called from N64Mem_Reset after Play_Init has populated scene data.  Returns 0 if THA consumption exceeds budget
// (should not happen on valid scenes).
// --------------------------------------------------------------------------------------------------------------------

u32 ArenaSizing_ComputeN64ArenaSize(PlayState* play, const VersionConstants* vc);

#ifdef __cplusplus
}
#endif

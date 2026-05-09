#pragma once

#include "z64.h"

#ifdef __cplusplus
extern "C" {



#endif

// --------------------------------------------------------------------------------------------------------------------
// Per-version constants that cannot be derived from existing OTR blobs at runtime.
//
// kaleidoOverlayVramSize requires the kaleido overlay table from the code segment (VRAM spans, not DMA sizes).
// Other former members have been replaced:
//   parameterStaticSize -> N64SizeData_GetDmaFileSize("parameter_static")
//   effectSsSize        -> N64_SIZEOF_EFFECT_SS (constant 0x60 across all N64 versions)
//
// #TODO: Extract kaleidoOverlayVramSize from the OTR exporter per ROM version.
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    u32 kaleidoOverlayVramSize; // max(ovl_kaleido_scope VRAM, ovl_player_actor VRAM)
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

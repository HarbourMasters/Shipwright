#pragma once

#include "z64.h"

#ifdef __cplusplus
extern "C" {



#endif

// --------------------------------------------------------------------------------------------------------------------
// Arena size computation
//
// Computes the N64 ZeldaArena size from first principles:
//  arena = THA_BUDGET - sum(all THA consumers at N64 sizes)
//
// All per-version constants are derived from OTR blobs at runtime:
//   kaleidoOverlayVramSize  ->  N64SizeData_GetKaleidoVramSize()   (misc/n64_memory/kaleido_vram_size)
//   parameterStaticSize     ->  N64SizeData_GetDmaFileSize(...)    (misc/n64_memory/dma_sizes)
//   effectSsSize            ->  N64_SIZEOF_EFFECT_SS               (constant 0x60, all N64 versions)
//
// Called from N64Mem_Reset after Play_Init has populated scene data.  Returns 0 if THA consumption exceeds budget
// (should not happen on valid scenes).
// --------------------------------------------------------------------------------------------------------------------

uint32_t ArenaSizing_ComputeN64ArenaSize(PlayState* play);

#ifdef __cplusplus
}
#endif
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {


#endif

// --------------------------------------------------------------------------------------------------------------------
// N64 size data loaded from the O2R archive.
//
// All data is extracted per-version by OTRExporter during ROM extraction.  The memory model targets N64 hardware
// uniformly -- ArenaNode size is hardcoded at 0x30.
//
// The remaining blobs carry ROM-specific data (e.g., overlay sizes, file sizes) rather than platform-specific
// behavior:
//  misc/n64_memory/dma_sizes              DMA file VROM sizes keyed by filename
//  misc/n64_memory/actor_overlay_sizes    Actor overlay VRAM sizes indexed by actor ID
//  misc/n64_memory/effect_overlay_sizes   Effect overlay VRAM sizes indexed by effect type
//  misc/n64_memory/actor_instance_sizes   Actor instance struct sizes indexed by actor ID
//  misc/n64_memory/kaleido_vram_size      max(ovl_kaleido_scope VRAM, ovl_player_actor VRAM)
//
// Each table is loaded lazily on first query and cached for the lifetime of the process.
// --------------------------------------------------------------------------------------------------------------------

uint32_t N64SizeData_GetDmaFileSize(const char* name);
uint32_t N64SizeData_GetActorOverlaySize(uint16_t actorId);
uint32_t N64SizeData_GetEffectOverlaySize(uint16_t effectType);
uint32_t N64SizeData_GetActorInstanceSize(uint16_t actorId);
uint32_t N64SizeData_GetKaleidoVramSize(void);

#ifdef __cplusplus
}
#endif
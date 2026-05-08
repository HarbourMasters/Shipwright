#pragma once

#include <libultraship/libultraship.h>

#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------------------------------------------------------
// N64 size data loaded from the OTR archive.
//
// All data is extracted per-version by OTRExporter during ROM extraction:
//  misc/dma_sizes              DMA file VROM sizes keyed by filename
//  misc/actor_overlay_sizes    Actor overlay VRAM sizes indexed by actor ID
//  misc/effect_overlay_sizes   Effect overlay VRAM sizes indexed by effect type
//  misc/actor_instance_sizes   Actor instance struct sizes indexed by actor ID
//
// Each table is loaded lazily on first query and cached for the lifetime of the process.
// --------------------------------------------------------------------------------------------------------------------

u32 N64SizeData_GetDmaFileSize(const char* name);
u32 N64SizeData_GetActorOverlaySize(u16 actorId);
u32 N64SizeData_GetEffectOverlaySize(u16 effectType);
u32 N64SizeData_GetActorInstanceSize(u16 actorId);

#ifdef __cplusplus
}
#endif
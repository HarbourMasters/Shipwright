#pragma once

#include <libultraship/libultraship.h>

#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------------------------------------------------------
// N64 size data loaded from the OTR archive.
//
// All data is extracted per-version by OTRExporter during ROM extraction:
//  misc/n64_memory/dma_sizes              DMA file VROM sizes keyed by filename
//  misc/n64_memory/actor_overlay_sizes    Actor overlay VRAM sizes indexed by actor ID
//  misc/n64_memory/effect_overlay_sizes   Effect overlay VRAM sizes indexed by effect type
//  misc/n64_memory/actor_instance_sizes   Actor instance struct sizes indexed by actor ID
//  misc/n64_memory/kaleido_vram_size      max(ovl_kaleido_scope VRAM, ovl_player_actor VRAM)
//  misc/n64_memory/arena_node_size        N64 ArenaNode size (0x10 retail, 0x30 debug)
//
// Each table is loaded lazily on first query and cached for the lifetime of the process.
// --------------------------------------------------------------------------------------------------------------------

u32 N64SizeData_GetDmaFileSize(const char* name);
u32 N64SizeData_GetActorOverlaySize(u16 actorId);
u32 N64SizeData_GetEffectOverlaySize(u16 effectType);
u32 N64SizeData_GetActorInstanceSize(u16 actorId);
u32 N64SizeData_GetKaleidoVramSize(void);
u32 N64SizeData_GetArenaNodeSize(void);
u32 N64SizeData_GetGiObjectSegmentSize(void);

#ifdef __cplusplus
}
#endif
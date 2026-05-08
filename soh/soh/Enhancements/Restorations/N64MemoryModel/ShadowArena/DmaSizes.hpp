#pragma once

#include <libultraship/libultraship.h>

#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------------------------------------------------------
// DMA file size lookup
//
// Queries the DMA file size table from the OTR (misc/dma_sizes).  Returns the VROM size (virtEnd - virtStart) for the
// given filename, or 0 if not found.  This table is loaded once on first call and cached.
// --------------------------------------------------------------------------------------------------------------------

u32 DmaSizes_GetFileSize(const char* name);

#ifdef __cplusplus
}
#endif

#pragma once

#include <libultraship/libultraship.h>

#ifdef __cplusplus
extern "C" {
#endif

// Reset state and cache CVar. Call after arena reinitialization.
void N64Mem_Reset(void);

s32 N64Mem_IsActive(void);

#ifdef __cplusplus
}
#endif

#pragma once

#include <stdint.h>
#include "soh/Enhancements/item-tables/ItemTableTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GIM_DISABLED,
    GIM_NTSC,
    GIM_PAL,
} GimVersion;

GetItemEntry Gim_RetrieveOobGetItemEntry(int16_t getItemId);

#ifdef __cplusplus
}
#endif

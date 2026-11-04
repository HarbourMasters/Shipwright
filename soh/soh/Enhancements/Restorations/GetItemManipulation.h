#pragma once

#include <stdint.h>
#include "soh/Enhancements/item-tables/ItemTableTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GIM_DISABLED,
    GIM_NTSC_1_0,
    GIM_NTSC_1_1,
    GIM_NTSC_1_2,
    GIM_PAL_1_0,
    GIM_PAL_1_1,
    GIM_GC_U,
    GIM_GC_E,
    GIM_GC_J,
    GIM_MQ_U,
    GIM_MQ_E,
    GIM_MQ_J,
    GIM_IQUE_CHN,
    GIM_IQUE_TWN,
    GIM_MQ_DEBUG,
    GIM_MZX_NTSC,
    GIM_MZX_PAL
} GimVersion;

GetItemEntry Gim_RetrieveOobGetItemEntry(int16_t getItemId);

#ifdef __cplusplus
}
#endif

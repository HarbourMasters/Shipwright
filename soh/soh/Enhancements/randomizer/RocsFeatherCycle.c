#include "soh/Enhancements/randomizer/RocsFeatherCycle.h"
#include "functions.h"
#include "variables.h"
#include "macros.h"

uint8_t Enhancement_GetNextNayrusItem() {
    if (INV_CONTENT(ITEM_NAYRUS_LOVE) == ITEM_NAYRUS_LOVE && Flags_GetRandomizerInf(RAND_INF_OBTAINED_ROCS_FEATHER)) {
        return ITEM_ROCS_FEATHER;
    }
    if (INV_CONTENT(ITEM_NAYRUS_LOVE) == ITEM_ROCS_FEATHER && Flags_GetRandomizerInf(RAND_INF_OBTAINED_NAYRUS_LOVE)) {
        return ITEM_NAYRUS_LOVE;
    }
    return ITEM_NONE;
}

uint8_t Enhancement_GetPrevNayrusItem() {
    if (INV_CONTENT(ITEM_NAYRUS_LOVE) == ITEM_NAYRUS_LOVE && Flags_GetRandomizerInf(RAND_INF_OBTAINED_ROCS_FEATHER)) {
        return ITEM_ROCS_FEATHER;
    }
    if (INV_CONTENT(ITEM_NAYRUS_LOVE) == ITEM_ROCS_FEATHER && Flags_GetRandomizerInf(RAND_INF_OBTAINED_NAYRUS_LOVE)) {
        return ITEM_NAYRUS_LOVE;
    }

    return ITEM_NONE;
}

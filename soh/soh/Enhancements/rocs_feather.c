#include "soh/Enhancements/rocs_feather.h"
#include "functions.h"
#include "variables.h"
#include "macros.h"

uint8_t Enhancement_GetNextNayrusItem() {
    if (INV_CONTENT(ITEM_NAYRUS_LOVE) == ITEM_NAYRUS_LOVE || INV_CONTENT(ITEM_NAYRUS_LOVE) == ITEM_NONE) {
        return ITEM_ROCS_FEATHER;
    }
    if (INV_CONTENT(ITEM_NAYRUS_LOVE) == ITEM_ROCS_FEATHER && gSaveContext.nayrusCollected) {
        return ITEM_NAYRUS_LOVE;
    }
    return ITEM_NONE;
}

uint8_t Enhancement_GetPrevNayrusItem() {
    if (INV_CONTENT(ITEM_NAYRUS_LOVE) == ITEM_NAYRUS_LOVE || INV_CONTENT(ITEM_NAYRUS_LOVE) == ITEM_NONE) {
        return ITEM_ROCS_FEATHER;
    }
    if (INV_CONTENT(ITEM_NAYRUS_LOVE) == ITEM_ROCS_FEATHER && gSaveContext.nayrusCollected) {
        return ITEM_NAYRUS_LOVE;
    }

    return ITEM_NONE;
}

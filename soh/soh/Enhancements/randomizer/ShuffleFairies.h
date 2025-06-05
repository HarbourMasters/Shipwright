#pragma once

#include <z64.h>
#include "soh/Enhancements/item-tables/ItemTableTypes.h"
#include "randomizerTypes.h"

typedef struct FairyIdentity {
    RandomizerInf randomizerInf;
    GetItemEntry itemEntry;
} FairyIdentity;

void ShuffleFairies_RegisterHooks();
void ShuffleFairies_UnregisterHooks();
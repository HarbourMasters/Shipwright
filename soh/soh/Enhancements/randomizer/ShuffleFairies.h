#pragma once

#include <z64.h>
#include "soh/Enhancements/item-tables/ItemTableTypes.h"
#include "randomizer_inf.h"

typedef struct FairyIdentity {
    RandomizerInf randomizerInf;
    GetItemEntry itemEntry;
} FairyIdentity;

void ShuffleFairies_RegisterHooks();
void ShuffleFairies_UnregisterHooks();
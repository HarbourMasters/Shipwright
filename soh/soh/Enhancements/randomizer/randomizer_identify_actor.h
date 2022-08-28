#pragma once
#include "randomizerTypes.h"
#include "randomizer_inf.h"
#include "z64.h"

typedef struct ScrubIdentity {
    RandomizerInf randomizerInf;
    RandomizerCheck randomizerCheck;
    GetItemID getItemId;
    int32_t itemPrice;
    bool isShuffled;
} ScrubIdentity;

typedef struct CowIdentity {
    RandomizerInf randomizerInf;
    RandomizerCheck randomizerCheck;
} CowIdentity;

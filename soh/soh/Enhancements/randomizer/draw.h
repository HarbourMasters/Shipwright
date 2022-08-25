#ifndef RANDODRAW_H
#define RANDODRAW_H
#pragma once

#include "../item-tables/ItemTableTypes.h"

typedef struct GlobalContext GlobalContext;

extern "C" void Randomizer_DrawSmallKey(GlobalContext* globalCtx, GetItemEntry* getItemEntry);
extern "C" void Randomizer_DrawBossKey(GlobalContext* globalCtx, GetItemEntry* getItemEntry);
extern "C" void Randomizer_DrawDoubleDefense(GlobalContext* globalCtx, GetItemEntry getItemEntry);

#endif

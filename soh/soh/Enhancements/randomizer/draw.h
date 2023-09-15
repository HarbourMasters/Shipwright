#ifndef RANDODRAW_H
#define RANDODRAW_H
#pragma once

#include "../item-tables/ItemTableTypes.h"

typedef struct PlayState PlayState;

extern "C" void Randomizer_DrawSmallKey(PlayState* play, GetItemEntry* getItemEntry);
extern "C" void Randomizer_DrawKeyRing(PlayState* play, GetItemEntry* getItemEntry);
extern "C" void Randomizer_DrawBossKey(PlayState* play, GetItemEntry* getItemEntry);
extern "C" void Randomizer_DrawDoubleDefense(PlayState* play, GetItemEntry getItemEntry);
extern "C" void Randomizer_DrawIceTrap(PlayState* play, GetItemEntry getItemEntry);

#endif

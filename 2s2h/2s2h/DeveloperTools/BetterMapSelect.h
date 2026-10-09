#ifndef BETTER_MAP_SELECT_H
#define BETTER_MAP_SELECT_H

#include "overlays/gamestates/ovl_select/z_select.h"
#include "gfxprint.h"

void BetterMapSelect_Init(MapSelectState* mapSelectState);
void BetterMapSelect_PrintMenu(MapSelectState* mapSelectState, GfxPrint* printer);
void BetterMapSelect_Update(MapSelectState* mapSelectState);

#endif
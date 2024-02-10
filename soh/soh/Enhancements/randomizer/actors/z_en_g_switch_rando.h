#ifndef SHIP_Z_EN_G_SWITCH_RANDO_H
#define SHIP_Z_EN_G_SWITCH_RANDO_H

#include "overlays/actors/ovl_En_G_Switch/z_en_g_switch.h"

#ifdef __cplusplus
extern "C" {
#endif

void EnGSwitch_Randomizer_SilverRupeeIdle(EnGSwitch *self, PlayState *play);
void EnGSwitch_Randomizer_Draw(Actor *thisx, PlayState *play);

#ifdef __cplusplus
};
#endif

#endif //SHIP_Z_EN_G_SWITCH_RANDO_H

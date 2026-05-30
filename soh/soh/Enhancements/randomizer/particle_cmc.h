#pragma once

#include <ship/utils/color.h>
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/item-tables/ItemTableTypes.h"

#ifndef PARTICLE_CMC_H
#define PARTICLE_CMC_H

typedef enum {
    COLOR_PRIMARY,
    COLOR_SECONDARY,
    COLOR_FLARE,
} ParticleCMCColorType;

#ifdef __cplusplus
extern "C" {
#endif

Color_RGBA8 Randomizer_GetParticleCMCColor(GetItemCategory colorIndex, ParticleCMCColorType colorType);

#ifdef __cplusplus
}
#endif

#endif
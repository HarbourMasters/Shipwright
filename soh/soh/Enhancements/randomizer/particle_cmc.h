#pragma once

#include "soh/OTRGlobals.h"

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
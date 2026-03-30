#pragma once

#include "soh/OTRGlobals.h"

#ifndef PARTICLE_CMC_H
#define PARTICLE_CMC_H

typedef enum {
    PARTICLE_MAJOR,
    PARTICLE_SKULLTULA_TOKEN,
    PARTICLE_SMALL_KEY,
    PARTICLE_BOSS_KEY,
    PARTICLE_HEALTH,
    PARTICLE_LESSER,
    PARTICLE_JUNK,
} ParticleCMCColor;

typedef enum {
    COLOR_PRIMARY,
    COLOR_SECONDARY,
    COLOR_FLARE,
} ParticleCMCColorType;

#ifdef __cplusplus
extern "C" {
#endif

Color_RGBA8 Randomizer_GetParticleCMCColor(ParticleCMCColor colorIndex, ParticleCMCColorType colorType);

#ifdef __cplusplus
}
#endif

#endif
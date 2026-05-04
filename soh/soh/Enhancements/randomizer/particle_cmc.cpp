#include "particle_cmc.h"

// Color of the circle for the particles
static Color_RGBA8 mainColors[7] = {
    { 255, 255, 255, 255 }, // Junk
    { 170, 50, 0, 255 },    // Lesser
    { 250, 0, 0, 255 },     // Health
    { 255, 255, 0, 255 },   // Boss Key
    { 180, 180, 180, 255 }, // Small Key
    { 0, 0, 0, 255 },       // Skulltula Token
    { 250, 185, 40, 255 },  // Major
};

// Secondary color of the circle for the particles
static Color_RGBA8 secColors[7] = {
    { 255, 255, 255, 255 }, // Junk
    { 250, 75, 0, 255 },    // Lesser
    { 0, 0, 255, 255 },     // Health
    { 0, 200, 255, 255 },   // Boss Key
    { 130, 130, 130, 255 }, // Small Key
    { 255, 250, 190, 255 }, // Skulltula Token
    { 255, 220, 135, 255 }, // Major
};

// Color of the faded flares stretching off the particles
static Color_RGBA8 flareColors[7] = {
    { 135, 135, 135, 255 }, // Junk
    { 255, 160, 100, 255 }, // Lesser
    { 255, 125, 125, 255 }, // Health
    { 0, 200, 255, 255 },   // Boss Key
    { 100, 100, 100, 255 }, // Small Key
    { 255, 255, 255, 255 }, // Skulltula Token
    { 250, 220, 180, 255 }, // Major
};

Color_RGBA8 Randomizer_GetParticleCMCColor(GetItemCategory colorIndex, ParticleCMCColorType colorType) {
    switch (colorType) {
        case COLOR_PRIMARY:
            return mainColors[colorIndex];
        case COLOR_SECONDARY:
            return secColors[colorIndex];
        case COLOR_FLARE:
            return flareColors[colorIndex];
        default:
            return mainColors[colorIndex];
    }
}
#pragma once

#include "randomizerTypes.h"

struct PlayState;

namespace Rando {

// Draws `soul` as a little figurine of its NPC, in the current item space. Returns false if the soul has
// no figurine (or its model couldn't be loaded), so the caller can draw the generic soul token instead.
bool DrawNpcSoulFigurine(PlayState* play, RandomizerGet soul);

} // namespace Rando

#pragma once

#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/randomizer/location.h"

typedef enum {
    FSO_SOURCE_RANDO,
    FSO_SOURCE_CVARS,
} FishsanityOptionsSource;

namespace Rando::Fishsanity {

/**
 * @brief Returns true if the given fish location is active
 *
 * @param loc The Location to check
 * @param optionsSource Optionally declare from which source to pull settings
 */
bool GetFishLocationIncluded(Rando::Location* loc, FishsanityOptionsSource optionsSource = FSO_SOURCE_RANDO);

/**
 * @brief Initializes pond fish state from save
 */
void InitializeFromSave();

} // namespace Rando::Fishsanity

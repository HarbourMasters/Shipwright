#pragma once

#include <libultraship/libultraship.h>
#include "vanilla-behavior/GIVanillaBehavior.h"
#include "GameInteractor.h"
#include <stdarg.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// MARK: - Gameplay
bool GameInteractor_Should(GIVanillaBehavior flag, uint32_t result, ...);

#ifdef __cplusplus
}
#endif

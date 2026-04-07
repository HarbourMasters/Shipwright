#pragma once

#include "vanilla-behavior/GIVanillaBehavior.h"
#include "GameInteractor.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

// MARK: - Gameplay
bool GameInteractor_Should(GIVanillaBehavior flag, uint32_t result, ...);

#ifdef __cplusplus
}
#endif

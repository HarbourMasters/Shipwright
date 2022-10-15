#pragma once

#include "stdint.h"

#define GRAVITY_LEVEL_NORMAL 1.0f
#define GRAVITY_LEVEL_LIGHT 0.0f
#define GRAVITY_LEVEL_HEAVY 2.0f

#ifdef __cplusplus
extern "C" {
#endif
// bools are exported as uint32_t for compatibility with C code
extern uint32_t chaosEffectNoUI;
extern uint32_t chaosEffectGiantLink;
extern uint32_t chaosEffectMinishLink;
extern uint32_t chaosEffectPaperLink;
extern uint32_t chaosEffectResetLinkScale;
extern uint32_t chaosEffectInvisibleLink;
extern uint32_t chaosEffectOneHitKO;
extern uint32_t chaosEffectPacifistMode;
extern int32_t chaosEffectDefenseModifier;
extern uint32_t chaosEffectNoZ;
extern uint32_t chaosEffectReverseControls;

extern uint32_t chaosEffectGravityLevel;
extern int32_t chaosEffectSpeedModifier;
#ifdef __cplusplus
}
#endif

void DebugConsole_Init(void);

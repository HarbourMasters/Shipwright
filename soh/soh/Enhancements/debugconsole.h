#pragma once

#include "stdint.h"

#define GRAVITY_LEVEL_NORMAL 1.0f
#define GRAVITY_LEVEL_LIGHT 0.0f
#define GRAVITY_LEVEL_HEAVY 2.0f

#ifdef __cplusplus
extern "C" {
#endif
// bools are exported as uint32_t for compatibility with C code
extern uint32_t noUI;
extern uint32_t giantLink;
extern uint32_t minishLink;
extern uint32_t paperLink;
extern uint32_t resetLinkScale;
extern uint32_t invisibleLink;
extern uint32_t oneHitKO;
extern uint32_t pacifistMode;
extern int32_t defenseModifier;
extern uint32_t noZ;
extern uint32_t reverseControls;

extern uint32_t gravityLevel;
extern int32_t speedModifier;
#ifdef __cplusplus
}
#endif

void DebugConsole_Init(void);

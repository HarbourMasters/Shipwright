#pragma once

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif
extern uint32_t noUI;
extern uint32_t giantLink;
extern uint32_t minishLink;
extern uint32_t paperLink;
extern uint32_t gravityLevel;
extern uint32_t resetLinkScale;
extern uint32_t invisibleLink;
extern uint32_t oneHitKO;
extern uint32_t pacifistMode;
extern int32_t defenseModifier;
extern uint32_t noZ;
extern uint32_t reverseControls;
extern int32_t speedModifier;
#ifdef __cplusplus
}
#endif

void DebugConsole_Init(void);

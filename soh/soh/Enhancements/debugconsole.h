#pragma once

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif
extern uint32_t giantLink;
extern uint32_t minishLink;
extern uint32_t gravityLevel;
extern uint32_t resetLinkScale;
extern uint32_t invisibleLink;
#ifdef __cplusplus
}
#endif

void DebugConsole_Init(void);

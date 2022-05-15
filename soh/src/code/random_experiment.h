#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void cryptoRandomInit(void);
unsigned long cryptoRandomUint32(unsigned long);

extern bool gCsprngInitialized;
extern unsigned long (*randomUint32)(unsigned long);
unsigned long standardRandomUint32(unsigned long);

#ifdef __cplusplus
}
#endif
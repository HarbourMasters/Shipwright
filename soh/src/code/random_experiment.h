#pragma once
#include <stdbool.h>

//TODO: implement equivalent CSPRNG on Linux and Unix platforms
#if defined(_WIN32)
bool cryptoRandomInit(void);
void cryptoRandomTearDown(void);
unsigned long cryptoRandomUint32(unsigned long);
#endif // _WIN32

extern bool CSPRNG_INITIALIZED;
extern unsigned long (*randomUint32)(unsigned long);
unsigned long standardRandomUint32(unsigned long);
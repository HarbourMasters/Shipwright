#ifndef LIBC64_SLEEP_H
#define LIBC64_SLEEP_H

#include <libultraship/libultra.h>

void Sleep_Cycles(OSTime cycles);
void Sleep_Nsec(u32 nsec);
void Sleep_Usec(u32 usec);
void Sleep_Msec(u32 ms);
void Sleep_Sec(u32 sec);

#endif
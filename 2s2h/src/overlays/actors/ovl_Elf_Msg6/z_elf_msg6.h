#ifndef Z_ELF_MSG6_H
#define Z_ELF_MSG6_H

#include "global.h"

struct ElfMsg6;

typedef void (*ElfMsg6ActionFunc)(struct ElfMsg6*, PlayState*);

#define ELFMSG6_GET_F(thisx) ((thisx)->params & 0xF)
#define ELFMSG6_GET_F0(thisx) (((thisx)->params & 0xF0) >> 4)
#define ELFMSG6_SWITCH_FLAG(thisx) (((thisx)->params & 0xFE00) >> 9)

typedef struct ElfMsg6 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ElfMsg6ActionFunc actionFunc;
} ElfMsg6; // size = 0x148

#endif // Z_ELF_MSG6_H

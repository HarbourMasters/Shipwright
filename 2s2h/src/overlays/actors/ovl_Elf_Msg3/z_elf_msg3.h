#ifndef Z_ELF_MSG3_H
#define Z_ELF_MSG3_H

#include "global.h"

#define ELFMSG3_GET_SWITCH_FLAG(thisx) (((thisx)->params & 0x7F00) >> 8)
#define ELFMSG3_GET_FF(thisx) ((thisx)->params & 0xFF)
#define ELFMSG3_GET_8000(thisx) ((thisx)->params & 0x8000)

struct ElfMsg3;

typedef void (*ElfMsg3ActionFunc)(struct ElfMsg3*, PlayState*);

typedef struct ElfMsg3 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ElfMsg3ActionFunc actionFunc;
} ElfMsg3; // size = 0x148

#endif // Z_ELF_MSG3_H

#ifndef Z_ELF_MSG2_H
#define Z_ELF_MSG2_H

#include "global.h"

#define ELFMSG2_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 8) & 0x7F)
#define ELFMSG2_GET_FF(thisx) ((thisx)->params & 0xFF)

struct ElfMsg2;

typedef void (*ElfMsg2ActionFunc)(struct ElfMsg2*, PlayState*);

typedef struct ElfMsg2 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ElfMsg2ActionFunc actionFunc;
} ElfMsg2; // size = 0x148

#endif // Z_ELF_MSG2_H

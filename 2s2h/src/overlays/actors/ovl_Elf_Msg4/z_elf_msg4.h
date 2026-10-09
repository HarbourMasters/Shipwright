#ifndef Z_ELF_MSG4_H
#define Z_ELF_MSG4_H

#include "global.h"

#define ELFMSG4_GET_SWITCH_FLAG(thisx) (((thisx)->params & 0x7F00) >> 8)
#define ELFMSG4_GET_8000(thisx) ((thisx)->params & 0x8000)
#define ELFMSG4_GET_TEXT(thisx) ((thisx)->params & 0xFF)

struct ElfMsg4;

typedef void (*ElfMsg4ActionFunc)(struct ElfMsg4*, PlayState*);

typedef struct ElfMsg4 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ Actor* elfMsg5;
    /* 0x148 */ ElfMsg4ActionFunc actionFunc;
} ElfMsg4; // size = 0x14C

#endif // Z_ELF_MSG4_H

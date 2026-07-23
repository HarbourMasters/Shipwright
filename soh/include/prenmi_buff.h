#ifndef PRENMI_BUFF_H
#define PRENMI_BUFF_H

#include <libultraship/libultra.h>
#include "alignment.h"

#include "extern_c_helper.h"

// This struct is used at osAppNMIBuffer which is not at an 8-byte aligned address. This causes an unaligned access
// crash if the OSTime variables use 64-bit load/store instructions, which is the case in any MIPS ABI other than O32
// where 64-bit load/store instructions are emulated with 2x 32-bit load/store instructions. The alignment attribute
// conveys that this structure will not always be 8-bytes aligned, allowing a modern compiler to generate non-crashing
// code for accessing these. This is not an issue in the original compiler as it only output O32 ABI code.
ALIGNED4 typedef struct PreNmiBuff {
    /* 0x00 */ u32 resetting;
    /* 0x04 */ u32 resetCount;
    /* 0x08 */ OSTime duration;
    /* 0x10 */ OSTime resetTime;
} PreNmiBuff; // size = 0x18 (actually osAppNmiBuffer is 0x40 bytes large but the rest is unused)

#ifdef __cplusplus
#define this thisx
#endif

EXTERN_C PreNmiBuff* gAppNmiBufferPtr;

EXTERN_C void PreNmiBuff_Init(PreNmiBuff* this);
EXTERN_C void PreNmiBuff_SetReset(PreNmiBuff* this);
EXTERN_C u32 PreNmiBuff_IsResetting(PreNmiBuff* this);

#ifdef __cplusplus
#undef this
#endif

#endif
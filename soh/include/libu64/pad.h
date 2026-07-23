#ifndef LIBU64_PAD_H
#define LIBU64_PAD_H

#include <libultraship/libultra.h>

#include "extern_c_helper.h"

typedef struct Input {
    /* 0x00 */ OSContPad cur;
    /* 0x06 */ OSContPad prev;
    /* 0x0C */ OSContPad press; // X/Y store delta from last frame
    /* 0x12 */ OSContPad rel; // X/Y store adjusted
} Input; // size = 0x18

EXTERN_C void PadUtils_Init(Input* input);
EXTERN_C void func_800FCB70(void);
EXTERN_C void PadUtils_ResetPressRel(Input* input);
EXTERN_C u32 PadUtils_CheckCurExact(Input* input, u16 value);
EXTERN_C u32 PadUtils_CheckCur(Input* input, u16 key);
EXTERN_C u32 PadUtils_CheckPressed(Input* input, u16 key);
EXTERN_C u32 PadUtils_CheckReleased(Input* input, u16 key);
EXTERN_C u16 PadUtils_GetCurButton(Input* input);
EXTERN_C u16 PadUtils_GetPressButton(Input* input);
EXTERN_C s8 PadUtils_GetCurX(Input* input);
EXTERN_C s8 PadUtils_GetCurY(Input* input);
EXTERN_C void PadUtils_SetRelXY(Input* input, s32 x, s32 y);
EXTERN_C s8 PadUtils_GetRelXImpl(Input* input);
EXTERN_C s8 PadUtils_GetRelYImpl(Input* input);
EXTERN_C s8 PadUtils_GetRelX(Input* input);
EXTERN_C s8 PadUtils_GetRelY(Input* input);
EXTERN_C void PadUtils_UpdateRelXY(Input* input);

#endif
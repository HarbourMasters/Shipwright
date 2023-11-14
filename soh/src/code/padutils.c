#include "global.h"

#include <string.h>

void PadUtils_Init(Input* input) {
    memset(input,0, sizeof(Input));
}

void func_800FCB70(void) {
}

void PadUtils_ResetPressRel(Input* input) {
    input->press.button = 0;
    input->rel.button = 0;
}

u32 PadUtils_CheckCurExact(Input* input, u16 value) {
    return value == input->cur.button;
}

u32 PadUtils_CheckCur(Input* input, u16 key) {
    return key == (input->cur.button & key);
}

u32 PadUtils_CheckPressed(Input* input, u16 key) {
    return key == (input->press.button & key);
}

u32 PadUtils_CheckReleased(Input* input, u16 key) {
    return key == (input->rel.button & key);
}

u16 PadUtils_GetCurButton(Input* input) {
    return input->cur.button;
}

u16 PadUtils_GetPressButton(Input* input) {
    return input->press.button;
}

s8 PadUtils_GetCurX(Input* input) {
    return input->cur.stick_x;
}

s8 PadUtils_GetCurY(Input* input) {
    return input->cur.stick_y;
}

void PadUtils_SetRelXY(Input* input, s32 x, s32 y) {
    input->rel.stick_x = x;
    input->rel.stick_y = y;
}

s8 PadUtils_GetRelXImpl(Input* input) {
    return input->rel.stick_x;
}

s8 PadUtils_GetRelYImpl(Input* input) {
    return input->rel.stick_y;
}

s8 PadUtils_GetRelX(Input* input) {
    return PadUtils_GetRelXImpl(input);
}

s8 PadUtils_GetRelY(Input* input) {
    return PadUtils_GetRelYImpl(input);
}

void PadUtils_UpdateRelXY(Input* input) {
    s32 curX = PadUtils_GetCurX(input);
    s32 curY = PadUtils_GetCurY(input);
    s32 relX;
    s32 relY;

    if (curX > 7) {
        relX = (curX < 0x43) ? curX - 7 : 0x43 - 7;
    } else if (curX < -7) {
        relX = (curX > -0x43) ? curX + 7 : -0x43 + 7;
    } else {
        relX = 0;
    }

    if (curY > 7) {
        relY = (curY < 0x43) ? curY - 7 : 0x43 - 7;

    } else if (curY < -7) {
        relY = (curY > -0x43) ? curY + 7 : -0x43 + 7;
    } else {
        relY = 0;
    }

    PadUtils_SetRelXY(input, relX, relY);
}

// #region SOH [Enhancement]
s8 PadUtils_GetCurRX(Input* input) {
    return input->cur.right_stick_x;
}

s8 PadUtils_GetCurRY(Input* input) {
    return input->cur.right_stick_y;
}

void PadUtils_SetRelRXY(Input* input, s32 x, s32 y) {
    input->rel.right_stick_x = x;
    input->rel.right_stick_y = y;
}

s8 PadUtils_GetRelRXImpl(Input* input) {
    return input->rel.right_stick_x;
}

s8 PadUtils_GetRelRYImpl(Input* input) {
    return input->rel.right_stick_y;
}

s8 PadUtils_GetRelRX(Input* input) {
    return PadUtils_GetRelRXImpl(input);
}

s8 PadUtils_GetRelRY(Input* input) {
    return PadUtils_GetRelRYImpl(input);
}

void PadUtils_UpdateRelRXY(Input* input) {
    s32 curX = PadUtils_GetCurRX(input);
    s32 curY = PadUtils_GetCurRY(input);
    s32 relX;
    s32 relY;

    if (curX > 7) {
        relX = (curX < 0x43) ? curX - 7 : 0x43 - 7;
    } else if (curX < -7) {
        relX = (curX > -0x43) ? curX + 7 : -0x43 + 7;
    } else {
        relX = 0;
    }

    if (curY > 7) {
        relY = (curY < 0x43) ? curY - 7 : 0x43 - 7;

    } else if (curY < -7) {
        relY = (curY > -0x43) ? curY + 7 : -0x43 + 7;
    } else {
        relY = 0;
    }

    PadUtils_SetRelRXY(input, relX, relY);
}
// #endregion

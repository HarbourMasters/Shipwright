#include "global.h"

const u8 sSfxBankIds[] = {
    BANK_PLAYER, BANK_ITEM, BANK_ENV, BANK_ENEMY, BANK_SYSTEM, BANK_OCARINA, BANK_VOICE,
};

void AudioMgr_StopAllSfxExceptSystem(void) {
    const u8* bankIdPtr;
    s32 dbgVar;

    // Remnant of debug
    dbgVar = 0;

    if (dbgVar != 2) {
        for (bankIdPtr = &sSfxBankIds[0]; bankIdPtr < (sSfxBankIds + ARRAY_COUNT(sSfxBankIds)); bankIdPtr++) {
            if ((dbgVar != 0) || (*bankIdPtr != BANK_SYSTEM)) {
                AudioSfx_StopByBank(*bankIdPtr);
            }
        }
    }
}

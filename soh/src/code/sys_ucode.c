#include "global.h"

#include "public/bridge/gfxbridge.h"

UcodeHandlers sDefaultGSPUCodeText = ucode_f3dex2;
// u64* sDefaultGSPUCodeData = gspF3DZEX2_NoN_PosLight_fifoDataStart;

uintptr_t SysUcode_GetUCodeBoot(void) {
    // return rspbootTextStart;
    return (uintptr_t)NULL;
}

size_t SysUcode_GetUCodeBootSize(void) {
    // return (ptrdiff_t)((uintptr_t)rspbootTextEnd - (uintptr_t)rspbootTextStart);
    return 0;
}

uint32_t SysUcode_GetUCode(void) {
    return sDefaultGSPUCodeText;
}

uintptr_t SysUcode_GetUCodeData(void) {
    // return sDefaultGSPUCodeData;
    return (uintptr_t)NULL;
}

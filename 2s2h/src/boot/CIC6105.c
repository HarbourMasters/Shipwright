#include "prevent_bss_reordering.h"
#include "CIC6105.h"
#include "build.h"
#include "fault.h"

s32 gCICAddr1Val;
s32 gCICAddr2Val;
FaultClient sRomInfoFaultClient;

void CIC6105_Noop1(void) {
}

void CIC6105_Noop2(void) {
}

void CIC6105_PrintRomInfo(void) {
    FaultDrawer_DrawText(80, 200, "SP_STATUS %08x", IO_READ(SP_STATUS_REG));
    FaultDrawer_DrawText(40, 184, "ROM_F [Creator:%s]", gBuildTeam);
    FaultDrawer_DrawText(56, 192, "[Date:%s]", gBuildDate);
}

void CIC6105_AddRomInfoFaultPage(void) {
    Fault_AddClient(&sRomInfoFaultClient, (void*)CIC6105_PrintRomInfo, NULL, NULL);
}

void CIC6105_Destroy(void) {
    Fault_RemoveClient(&sRomInfoFaultClient);
}

void CIC6105_Init(void) {
    gCICAddr1Val = IO_READ(CIC_ADDRESS_1);
    gCICAddr2Val = IO_READ(CIC_ADDRESS_2);
}

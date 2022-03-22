#include "global.h"
#include "ultra64/internal.h"

void __osDevMgrMain(void* arg) {
    OSIoMesg* ioMesg;
    OSMesg sp70;
    OSMesg sp6C;
    OSMgrArgs* arg0 = (OSMgrArgs*)arg;
    __OSTranxInfo* transfer;
    __OSBlockInfo* block;
    s32 phi_s2;
    s32 phi_s0;
    u32 sp54;

    ioMesg = NULL;

    while (true) {
        osRecvMesg(arg0->cmdQueue, (OSMesg)&ioMesg, OS_MESG_BLOCK);
        if ((ioMesg->piHandle != NULL) && (ioMesg->piHandle->type == DEVICE_TYPE_64DD) &&
            ((ioMesg->piHandle->transferInfo.cmdType == 0) || (ioMesg->piHandle->transferInfo.cmdType == 1))) {
            transfer = &ioMesg->piHandle->transferInfo;
            block = &transfer->block[transfer->blockNum];
            transfer->sectorNum = -1;
            if (transfer->transferMode != 3) {
                block->dramAddr = (void*)((uintptr_t)block->dramAddr - block->sectorSize);
            }

            phi_s2 = ((transfer->transferMode == 2) && (ioMesg->piHandle->transferInfo.cmdType == 0)) ? 1 : 0;

            osRecvMesg(arg0->acccessQueue, &sp6C, OS_MESG_BLOCK);
            __osResetGlobalIntMask(0x00100401);
            __osEPiRawWriteIo(ioMesg->piHandle, 0x05000510, transfer->bmCtlShadow | 0x80000000);

            while (true) {
                osRecvMesg(arg0->eventQueue, &sp70, OS_MESG_BLOCK);
                transfer = &ioMesg->piHandle->transferInfo;
                block = &transfer->block[transfer->blockNum];
                if (block->errStatus == 0x1D) {
                    __osEPiRawWriteIo(ioMesg->piHandle, 0x05000510, transfer->bmCtlShadow | 0x10000000);
                    __osEPiRawWriteIo(ioMesg->piHandle, 0x05000510, transfer->bmCtlShadow);
                    __osEPiRawReadIo(ioMesg->piHandle, 0x05000508, &sp54);
                    if (sp54 & 0x02000000) {
                        __osEPiRawWriteIo(ioMesg->piHandle, 0x05000510, transfer->bmCtlShadow | 0x1000000);
                    }
                    block->errStatus = 4;
                    HW_REG(PI_STATUS_REG, u32) = PI_STATUS_CLEAR_INTR;
                    __osSetGlobalIntMask(0x00100C01);
                }
                osSendMesg(ioMesg->hdr.retQueue, ioMesg, OS_MESG_NOBLOCK);

                if ((phi_s2 != 1) || (ioMesg->piHandle->transferInfo.block[0].errStatus != 0)) {
                    break;
                }

                phi_s2 = 0;
            }

            osSendMesg(arg0->acccessQueue, 0, OS_MESG_NOBLOCK);
            if (ioMesg->piHandle->transferInfo.blockNum == 1) {
                osYieldThread();
            }
        } else {
            switch (ioMesg->hdr.type) {
                case OS_MESG_TYPE_DMAREAD:
                    osRecvMesg(arg0->acccessQueue, &sp6C, OS_MESG_BLOCK);
                    phi_s0 = arg0->piDmaCallback(OS_READ, ioMesg->devAddr, ioMesg->dramAddr, ioMesg->size);
                    break;
                case OS_MESG_TYPE_DMAWRITE:
                    osRecvMesg(arg0->acccessQueue, &sp6C, OS_MESG_BLOCK);
                    phi_s0 = arg0->piDmaCallback(OS_WRITE, ioMesg->devAddr, ioMesg->dramAddr, ioMesg->size);
                    break;
                case OS_MESG_TYPE_EDMAREAD:
                    osRecvMesg(arg0->acccessQueue, &sp6C, OS_MESG_BLOCK);
                    phi_s0 = arg0->epiDmaCallback(ioMesg->piHandle, OS_READ, ioMesg->devAddr, ioMesg->dramAddr,
                                                  ioMesg->size);
                    break;
                case OS_MESG_TYPE_EDMAWRITE:
                    osRecvMesg(arg0->acccessQueue, &sp6C, OS_MESG_BLOCK);
                    phi_s0 = arg0->epiDmaCallback(ioMesg->piHandle, OS_WRITE, ioMesg->devAddr, ioMesg->dramAddr,
                                                  ioMesg->size);
                    break;
                case OS_MESG_TYPE_LOOPBACK:
                    osSendMesg(ioMesg->hdr.retQueue, ioMesg, OS_MESG_NOBLOCK);
                    phi_s0 = -1;
                    break;
                default:
                    phi_s0 = -1;
                    break;
            }

            if (phi_s0 == 0) {
                osRecvMesg(arg0->eventQueue, &sp70, OS_MESG_BLOCK);
                osSendMesg(ioMesg->hdr.retQueue, ioMesg, OS_MESG_NOBLOCK);
                osSendMesg(arg0->acccessQueue, NULL, OS_MESG_NOBLOCK);
            }
        }
    }
}

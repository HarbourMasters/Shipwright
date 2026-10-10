#include "ultra64.h"
#include "libc/stdbool.h"

void __osDevMgrMain(void* arg) {
    OSIoMesg* ioMesg;
    OSMesg sp70;
    OSMesg sp6C; // "dummy"
    s32 ret;
    OSDevMgr* devMgr;
    s32 msgVar = 0;

    devMgr = (OSDevMgr*)arg;
    ioMesg = NULL;
    ret = 0;

    while (true) {
        osRecvMesg(devMgr->cmdQueue, (OSMesg*)&ioMesg, OS_MESG_BLOCK);
        if ((ioMesg->piHandle != NULL) && (ioMesg->piHandle->type == 2) &&
            ((ioMesg->piHandle->transferInfo.cmdType == OS_READ) ||
             (ioMesg->piHandle->transferInfo.cmdType == OS_WRITE))) {
            __OSBlockInfo* block;
            __OSTranxInfo* transfer;

            transfer = &ioMesg->piHandle->transferInfo;
            block = &transfer->block[transfer->blockNum];
            transfer->sectorNum = -1;
            if (transfer->transferMode != 3) {
                block->dramAddr = (void*)((u32)block->dramAddr - block->sectorSize);
            }

            if ((transfer->transferMode == 2) && (ioMesg->piHandle->transferInfo.cmdType == OS_READ)) {
                msgVar = 1;
            } else {
                msgVar = 0;
            }

            osRecvMesg(devMgr->acsQueue, &sp6C, OS_MESG_BLOCK);
            __osResetGlobalIntMask(OS_IM_PI);
            __osEPiRawWriteIo(ioMesg->piHandle, LEO_BM_CTL, transfer->bmCtlShadow | LEO_BM_CTL_START);

        readblock1:
            osRecvMesg(devMgr->evtQueue, &sp70, OS_MESG_BLOCK);
            transfer = &ioMesg->piHandle->transferInfo;
            block = &transfer->block[transfer->blockNum];
            if (block->errStatus == 0x1D) {
                u32 status;

                __osEPiRawWriteIo(ioMesg->piHandle, LEO_BM_CTL, transfer->bmCtlShadow | LEO_BM_CTL_RESET);
                __osEPiRawWriteIo(ioMesg->piHandle, LEO_BM_CTL, transfer->bmCtlShadow);
                __osEPiRawReadIo(ioMesg->piHandle, LEO_STATUS, &status);
                if (status & LEO_STATUS_MECHANIC_INTERRUPT) {
                    __osEPiRawWriteIo(ioMesg->piHandle, LEO_BM_CTL,
                                      transfer->bmCtlShadow | LEO_BM_CTL_CLR_MECHANIC_INTR);
                }
                block->errStatus = 4;
                IO_WRITE(PI_STATUS_REG, PI_CLR_INTR);
                __osSetGlobalIntMask(OS_IM_PI | SR_IBIT4);
            }
            osSendMesg(ioMesg->hdr.retQueue, (OSMesg)ioMesg, OS_MESG_NOBLOCK);

            if ((msgVar == 1) && (ioMesg->piHandle->transferInfo.block[0].errStatus == 0)) {
                msgVar = 0;
                goto readblock1;
            }

            osSendMesg(devMgr->acsQueue, NULL, OS_MESG_NOBLOCK);
            if (ioMesg->piHandle->transferInfo.blockNum == 1) {
                osYieldThread();
            }
        } else {
            switch (ioMesg->hdr.type) {
                case 11:
                    osRecvMesg(devMgr->acsQueue, &sp6C, OS_MESG_BLOCK);
                    ret = devMgr->piDmaCallback(OS_READ, ioMesg->devAddr, ioMesg->dramAddr, ioMesg->size);
                    break;
                case 12:
                    osRecvMesg(devMgr->acsQueue, &sp6C, OS_MESG_BLOCK);
                    ret = devMgr->piDmaCallback(OS_WRITE, ioMesg->devAddr, ioMesg->dramAddr, ioMesg->size);
                    break;
                case 15:
                    osRecvMesg(devMgr->acsQueue, &sp6C, OS_MESG_BLOCK);
                    ret = devMgr->epiDmaCallback(ioMesg->piHandle, OS_READ, ioMesg->devAddr, ioMesg->dramAddr,
                                                 ioMesg->size);
                    break;
                case 16:
                    osRecvMesg(devMgr->acsQueue, &sp6C, OS_MESG_BLOCK);
                    ret = devMgr->epiDmaCallback(ioMesg->piHandle, OS_WRITE, ioMesg->devAddr, ioMesg->dramAddr,
                                                 ioMesg->size);
                    break;
                case 10:
                    osSendMesg(ioMesg->hdr.retQueue, ioMesg, OS_MESG_NOBLOCK);
                    ret = -1;
                    break;
                default:
                    ret = -1;
                    break;
            }

            if (ret == 0) {
                osRecvMesg(devMgr->evtQueue, &sp70, OS_MESG_BLOCK);
                osSendMesg(ioMesg->hdr.retQueue, ioMesg, OS_MESG_NOBLOCK);
                osSendMesg(devMgr->acsQueue, NULL, OS_MESG_NOBLOCK);
            }
        }
    }
}

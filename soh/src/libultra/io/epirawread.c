#include "global.h"

s32 __osEPiRawReadIo(OSPiHandle* handle, u32 devAddr, u32* data) {
    s32 status;
    OSPiHandle* curHandle;

    while (status = HW_REG(PI_STATUS_REG, u32), status & (PI_STATUS_BUSY | PI_STATUS_IOBUSY)) {
        ;
    }

    if (__osCurrentHandle[handle->domain]->type != handle->type) {
        curHandle = __osCurrentHandle[handle->domain];

        if (handle->domain == 0) {
            if (curHandle->latency != handle->latency) {
                HW_REG(PI_BSD_DOM1_LAT_REG, u32) = handle->latency;
            }

            if (curHandle->pageSize != handle->pageSize) {
                HW_REG(PI_BSD_DOM1_PGS_REG, u32) = handle->pageSize;
            }

            if (curHandle->relDuration != handle->relDuration) {
                HW_REG(PI_BSD_DOM1_RLS_REG, u32) = handle->relDuration;
            }

            if (curHandle->pulse != handle->pulse) {
                HW_REG(PI_BSD_DOM1_PWD_REG, u32) = handle->pulse;
            }
        } else {
            if (curHandle->latency != handle->latency) {
                HW_REG(PI_BSD_DOM2_LAT_REG, u32) = handle->latency;
            }

            if (curHandle->pageSize != handle->pageSize) {
                HW_REG(PI_BSD_DOM2_PGS_REG, u32) = handle->pageSize;
            }

            if (curHandle->relDuration != handle->relDuration) {
                HW_REG(PI_BSD_DOM2_RLS_REG, u32) = handle->relDuration;
            }

            if (curHandle->pulse != handle->pulse) {
                HW_REG(PI_BSD_DOM2_PWD_REG, u32) = handle->pulse;
            }
        }

        curHandle->type = handle->type;
        curHandle->latency = handle->latency;
        curHandle->pageSize = handle->pageSize;
        curHandle->relDuration = handle->relDuration;
        curHandle->pulse = handle->pulse;
    }

    *data = HW_REG(handle->baseAddress | devAddr | 0, u32);
    return 0;
}

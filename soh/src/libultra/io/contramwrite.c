#include "ultra64.h"
#include "global.h"

s32 __osContRamWrite(OSMesgQueue* mq, s32 channel, u16 address, u8* buffer, s32 force) {
    s32 ret = 0;
    s32 i;
    u8* ptr;
    s32 retry = 2;
    u8 crc;

    if ((force != PFS_FORCE) && (address < PFS_LABEL_AREA) && (address != 0)) {
        return 0;
    }

    __osSiGetAccess();

    do {
        ptr = (u8*)(&gPifMempakBuf);

        if (__osContLastPoll != CONT_CMD_WRITE_MEMPACK || __osPfsLastChannel != channel) {
            __osContLastPoll = CONT_CMD_WRITE_MEMPACK;
            __osPfsLastChannel = channel;

            // clang-format off
            for (i = 0; i < channel; i++) { *ptr++ = 0; }
            // clang-format on

            gPifMempakBuf.status = 1;

            ((__OSContRamHeader*)ptr)->unk_00 = 0xFF;
            ((__OSContRamHeader*)ptr)->txsize = 35;
            ((__OSContRamHeader*)ptr)->rxsize = 1;
            ((__OSContRamHeader*)ptr)->poll = CONT_CMD_WRITE_MEMPACK;
            ((__OSContRamHeader*)ptr)->datacrc = 0xFF;

            ptr[sizeof(__OSContRamHeader)] = CONT_CMD_END;
        } else {
            ptr += channel;
        }
        ((__OSContRamHeader*)ptr)->hi = address >> 3;
        ((__OSContRamHeader*)ptr)->lo = ((address << 5) | __osContAddressCrc(address));

        bcopy(buffer, ((__OSContRamHeader*)ptr)->data, BLOCKSIZE);

        ret = __osSiRawStartDma(OS_WRITE, &gPifMempakBuf);
        crc = __osContDataCrc(buffer);
        osRecvMesg(mq, (OSMesg*)NULL, OS_MESG_BLOCK);

        ret = __osSiRawStartDma(OS_READ, &gPifMempakBuf);
        osRecvMesg(mq, (OSMesg*)NULL, OS_MESG_BLOCK);

        ret = ((((__OSContRamHeader*)ptr)->rxsize & 0xC0) >> 4);
        if (!ret) {
            if (crc != ((__OSContRamHeader*)ptr)->datacrc) {
                if ((ret = __osPfsGetStatus(mq, channel))) {
                    break;
                } else {
                    ret = PFS_ERR_CONTRFAIL;
                }
            }
        } else {
            ret = PFS_ERR_NOPACK;
        }
    } while ((ret == PFS_ERR_CONTRFAIL) && (retry-- >= 0));
    __osSiRelAccess();

    return ret;
}
